//
// @ Create by CaiZhili on 2017/7/18
// @ Brief
//

#include "connector.h"
#include "mem_check.h"

#define UNUSE(x) (void)(x);

using namespace std::placeholders;
using namespace nf;

unsigned long Connector::id_cnt_ = 1;

Connector::Connector(EventService &es, int fd)
        : cid_(GetIdCnt()),
          is_close_(0),
          last_act_time_(0),
          recv_buf_(DEFAULT_BUFF_SIZE),
          send_buf_(DEFAULT_BUFF_SIZE),
          task_(new IOTask(es, fd, 0)),
          socket_(fd) {
    SetLastActTimeToNow();
}

Connector::~Connector() {
    Close();
    if (task_)
        delete task_;
    task_ = NULL;
}

void Connector::Open(const ConnCbData &cb_data) {
    rdata_ = cb_data;

    task_->SetMask(EV_READABLE);
    task_->Bind(std::bind(&Connector::OnRead, this, _1, _2, _3));
    task_->Start();
}

void Connector::Send(const char *buff,
                     const size_t lenth,
                     const ConnCbData &cb_data) {
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
        ssize_t remain = recv_buf_.RemainSize();
        if (remain == 0) {
            nr = 0;
            break;
        }

        char *base = recv_buf_.TailPos();
        nr = socket_.Recv(base, remain); //InnerRead(base, remain);
        if (nr < 0) {
            snprintf(err_msg_, sizeof(err_msg_), "%s", socket_.GetErrMsg());
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

    memcpy(buff, base, take_size);
    recv_buf_.FrontAdvancing(take_size);

    return take_size;
}

Connector::ssize_t Connector::Send(const void *buff, const size_t size) {
    ssize_t nw = socket_.Send(buff, size);//InnerWrite(buff, size);
    if (nw < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", socket_.GetErrMsg());
        return FAIL;
    }

    //nw = 0 或 nw 小于要求写的大小，说明 socket 缓冲区满了。
    //缓存下未发送的数据
    if ((size_t) nw < size) {
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
    ssize_t nw = socket_.Send(buff, lenth);//InnerWrite(buff, lenth);
    if (nw < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", socket_.GetErrMsg());
        return FAIL;
    }
    send_buf_.FrontAdvancing(nw);

    return nw;
}

void Connector::Close() {
    if (is_close_)
        return;

    //To make close operate
    task_->Stop();
    socket_.Close();

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

Socket &Connector::GetSocket() {
    return socket_;
}

std::string Connector::GetErrMsg() {
    return std::string(err_msg_);
}

unsigned long Connector::GetIdCnt() {
    return id_cnt_++;
}
