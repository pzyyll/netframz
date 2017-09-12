//
// @ Create by CaiZhili on 2017/7/18
// @ Brief
//

#include "connector.h"
#include "mem_check.h"
#include "log.h"

#define UNUSE(x) (void)(x);

using namespace std::placeholders;
using namespace nf;

unsigned long Connector::id_cnt_ = 1;

Connector::Connector(EventService &es, int fd)
        : cid_(id_cnt_++),
          is_close_(0),
          last_act_time_(0),
          recv_buf_(DEFAULT_BUFF_SIZE),
          send_buf_(DEFAULT_BUFF_SIZE),
          task_(new IOTask(es, fd, 0)) {
    SetLastActTimeToNow();
}

Connector::~Connector() {
    Close();
    if (task_)
        delete task_;
    task_ = NULL;
}

void Connector::BeginRecv(const ConnCbData &cb_data) {
    rdata_ = cb_data;

    task_->SetMask(EV_READABLE);
    task_->Bind(std::bind(&Connector::OnRead, this, _1, _2, _3));
    task_->Start();
}

void Connector::Send(const char *buff,
                     const size_t lenth,
                     const ConnCbData &cb_data) {
    LockGuard lock(mutex_);
    ErrCode err_code(ErrCode::SUCCESS);

    ssize_t ns = Send((void *)buff, lenth);
    if (ns < 0) {
        err_code.set_ret(ErrCode::FAIL);
        err_code.set_err_msg(err_msg_);
        if (cb_data.handler)
            cb_data.handler(0, cb_data.pri_data, err_code);
        return;
    }

    //Socket 缓冲区满
    if ((size_t) ns < lenth) {
        wdata_ = cb_data;
        task_->SetMask(EV_WRITEABLE);
        task_->Bind(std::bind(&Connector::OnWriteRemain, this, _1, _2, _3));
        task_->Restart();
        return;
    }

    if (cb_data.handler)
        cb_data.handler(0, cb_data.pri_data, err_code);
}

void Connector::OnRead(EventService *es, task_data_t data, int mask) {
    ErrCode err_code(ErrCode::SUCCESS);
    ssize_t nr = 0;
    UNUSE(es);
    UNUSE(data);

    do {
        if (CheckMask(mask) < 0) {
            err_code.set_ret(ErrCode::FAIL);
            err_code.set_err_msg(err_msg_);
            nr = 0;
            break;
        }

        //检查用户缓冲区是否满了
        recv_buf_.MemoryMove2Left();
        log_debug("tpos|%lu, fpos|%lu", recv_buf_.tpos, recv_buf_.fpos);
        ssize_t remain = recv_buf_.RemainSize();
        if (remain == 0) {
            nr = 0;
            break;
        }

        char *base = recv_buf_.TailPos();
        nr = InnerRead(base, remain);
        if (nr < 0) {
            err_code.set_ret(ErrCode::FAIL);
            err_code.set_err_msg(err_msg_);
            nr = 0;
        }
    } while (false);

    recv_buf_.TailAdvancing(nr);

    if (rdata_.handler)
        rdata_.handler(recv_buf_.UsedSize(), rdata_.pri_data, err_code);
}

void Connector::OnWriteRemain(EventService *es, task_data_t data, int mask) {
    ErrCode err_code(ErrCode::FAIL);
    ssize_t ns = 0;
    UNUSE(es);
    UNUSE(data);

    do {
        if (CheckMask(mask) < 0) {
            err_code.set_err_msg(err_msg_);
            break;
        }

        ns = SendRemain();
        if (ns < 0) {
            err_code.set_err_msg(err_msg_);
            ns = 0;
            break;
        }

        if (send_buf_.UsedSize() == 0) {
            err_code.set_ret(ErrCode::SUCCESS);
            task_->SetMask(EV_READABLE);
            task_->Bind(std::bind(&Connector::OnRead, this, _1, _2, _3));
            task_->Restart();
            break;
        }

        log_debug("ns|%ld, remain|%d.", ns, (int)send_buf_.UsedSize());
        log_debug("tpos|%lu, fpos|%lu", send_buf_.tpos, send_buf_.fpos);

        return;
    } while (false);

    if (wdata_.handler)
            wdata_.handler(ns, wdata_.pri_data, err_code);
    //todo
}

int Connector::CheckMask(int mask) {
    if (mask & EV_RDHUP) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect already closed by foreign.");
        return FAIL;
    }

    if (mask & EV_HUP) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect close cause hup.");
        return FAIL;
    }

    if (mask & EV_ERR) {
        snprintf(err_msg_, sizeof(err_msg_), "Connect close cause happen err.");
        return FAIL;
    }

    return SUCCESS;
}

Connector::size_t Connector::Recv(void *buff, const size_t size) {
    size_t remain = recv_buf_.UsedSize();
    size_t take_size = size;
    char *base = recv_buf_.FrontPos();

    if (take_size > remain) {
        take_size = remain;
    }

    log_debug("remain|%lu, take|%lu", remain, take_size);
    memcpy(buff, base, take_size);
    recv_buf_.FrontAdvancing(take_size);

    return take_size;
}

Connector::ssize_t Connector::Send(const void *buff, const size_t size) {
    ssize_t nw = InnerWrite(buff, size);
    if (nw < 0) {
        return FAIL;
    }

    //nw = 0 或 nw 小于要求写的大小，说明 socket 缓冲区满了。
    //缓存下未发送的数据
    if ((size_t) nw < size) {
        log_debug("send full.");
        //检查用户缓存是否足够存放现场
        send_buf_.MemoryMove2Left();
        int snd_buf_size = send_buf_.RemainSize();
        int nleft = size - nw;
        if (snd_buf_size < nleft) {
            snprintf(err_msg_, sizeof(err_msg_),
                     "user buff is not enough. size(%d) < rsize(%d)",
                     snd_buf_size, nleft);
            return FAIL;
        }

        memcpy(send_buf_.TailPos(), (char *) buff + nw, nleft);
        send_buf_.TailAdvancing(nleft);
    }

    return nw;
}

Connector::ssize_t Connector::SendRemain() {
    if (send_buf_.UsedSize() == 0) {
        return 0;
    }

    void *buff = send_buf_.FrontPos();
    size_t lenth = send_buf_.UsedSize();
    ssize_t nw = InnerWrite(buff, lenth);
    if (nw < 0) {
        return FAIL;
    }
    send_buf_.FrontAdvancing(nw);
    log_debug("tpos|%lu, fpos|%lu, nw|%ld", send_buf_.tpos, send_buf_.fpos, nw);

    return nw;
}

void Connector::Close() {
    if (is_close_)
        return;

    //To make close operate
    task_->Stop();
    int fd = task_->GetFd();
    if (fd > 0)
        ::close(fd);

    is_close_ = true;
}

bool Connector::IsTimeOut(const unsigned long time_limit) {
    struct timeval now;
    gettimeofday(&now, NULL);
    if ((unsigned long) now.tv_sec - last_act_time_ >= time_limit) {
        return true;
    }

    return false;
}

void Connector::SetLastActTimeToNow() {
    struct timeval now;
    gettimeofday(&now, NULL);
    last_act_time_ = now.tv_sec;
}

struct Buffer &Connector::GetRecvBuff() {
    return recv_buf_;
}

unsigned long Connector::GetCID() {
    return cid_;
}

std::string Connector::GetErrMsg() {
    return std::string(err_msg_);
}

int Connector::MakeNonBlock(int fd) {
    int val = ::fcntl(fd, F_GETFL, 0);
    if (val < 0) {
        snprintf(err_msg_, sizeof(err_msg_),
                 "get fd(%d) status fail. %s", fd, strerror(errno));
        return FAIL;
    }

    val |= O_NONBLOCK;
    if (::fcntl(fd, F_SETFL, val) < 0) {
        snprintf(err_msg_, sizeof(err_msg_),
                 "set fd(%d) status fail. %s", fd, strerror(errno));
        return FAIL;
    }

    return SUCCESS;
}

Connector::ssize_t Connector::InnerRead(void *buff, size_t size) {
    int fd = task_->GetFd();
    ssize_t nread = 0;
    size_t nleft = size;
    char *ptr = (char *) buff;

    while (nleft > 0) {
        nread = ::read(fd, ptr, nleft);
        if (nread < 0) {
            if (EINTR == errno) {
                continue;
            }
            if (EAGAIN != errno) {
                snprintf(err_msg_, sizeof(err_msg_), "read(%d) happen error. %s",
                         fd, strerror(errno));
                return FAIL;
            }
            break;
        } else if (nread == 0) {
            break;
        }

        nleft -= nread;
        ptr += nread;
    }

    return (ssize_t)(size - nleft);
}

Connector::ssize_t Connector::InnerWrite(const void *buff, const size_t size) {
    int fd = task_->GetFd();
    ssize_t nwriten = 0;
    size_t nleft = size;
    const char *ptr = (const char *) buff;

    while (nleft > 0) {
        nwriten = ::write(fd, ptr, nleft);
        if (nwriten <= 0) {
            if (EINTR == errno) {
                continue;
            }
            if (EAGAIN != errno) {
                snprintf(err_msg_, sizeof(err_msg_), "write(%d) happen err. %s",
                         fd, strerror(errno));
                return FAIL;
            }

            break;
        }

        nleft -= nwriten;
        ptr += nwriten;
    }

    return (ssize_t)(size - nleft);
}
