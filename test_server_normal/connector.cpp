//
// @ Create by CaiZhili on 2017/7/18
// @ Bref
//
#include "connector.h"

unsigned long Connector::id_cnt_ = 1;

Connector::Connector(EventLoop &loop, int fd)
        : cid_(id_cnt_++),
          is_close_(0),
          last_act_time_(0),
          recv_buf_(DEFAULT_BUFF_SIZE),
          send_buf_(DEFAULT_BUFF_SIZE),
          task_(new IOTask(loop, fd, 0)) {
    SetLastActTimeToNow();
}

Connector::~Connector() {
    Close();
    if (task_)
        delete task_;
    task_ = NULL;
}

Connector::ssize_t Connector::Read(char *buff, const size_t size) {
    return InnerRead((void *) buff, size);
}

Connector::ssize_t Connector::Write(const char *buff, const size_t size) {
    ssize_t nw = InnerWrite((void *) buff, size);
    if (nw < 0) {
        return FAIL;
    }

    //nw = 0 或 nw 小于要求写的大小，说明 socket 缓冲区满了。
    //缓存下未发送的数据
    if ((size_t) nw < size) {
        //检查用户缓存是否足够存放现场
        int snd_buf_size = send_buf_.lenth - send_buf_.tpos;
        int nleft = size - nw;
        if (snd_buf_size < nleft) {
            snprintf(err_msg_, sizeof(err_msg_),
                     "user buff is not enough. size(%d) < rsize(%d)",
                     snd_buf_size, nleft);
            return FAIL;
        }
        memcpy(send_buf_.base + send_buf_.tpos, buff + nw, nleft);
        send_buf_.tpos += nleft;
    }
    return nw;
}

Connector::ssize_t Connector::WriteRemain() {
    if (send_buf_.fpos == send_buf_.tpos) {
        return 0;
    }

    void *buff = send_buf_.base;
    size_t lenth = send_buf_.tpos - send_buf_.fpos;
    ssize_t nw = InnerWrite(buff, lenth);
    send_buf_.fpos += nw;

    if (send_buf_.fpos == send_buf_.tpos) {
        send_buf_.fpos = send_buf_.tpos = 0;
    } else {
        //keep left aligned
        memmove(send_buf_.base, send_buf_.base + send_buf_.fpos, lenth - nw);
        send_buf_.fpos = 0;
        send_buf_.tpos -= nw;
    }

    return nw;
}

void Connector::Close() {
    if (is_close_)
        return;

    //To make close operate
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

Connector::size_t Connector::GetRemainSize() {
    return (send_buf_.tpos - send_buf_.fpos);
}

IOTask &Connector::GetIOTask() {
    return *task_;
}

unsigned long Connector::GetCID() {
    return cid_;
}

//================== protected or private ===============================//

int Connector::MakeFdBlockIs(bool is_block, int fd) {
    int val = ::fcntl(fd, F_GETFL, 0);
    if (val < 0) {
        snprintf(err_msg_, sizeof(err_msg_),
                 "get fd(%d) status fail. %s", fd, strerror(errno));
        return FAIL;
    }

    if (is_block) {
        val &= ~O_NONBLOCK;
    } else {
        val |= O_NONBLOCK;
    }

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
        }

        nleft -= nread;
        ptr += nread;
    }

    return (size - nleft);
}

Connector::ssize_t Connector::InnerWrite(const void *buff, const size_t size) {
    int fd = task_->GetFd();
    ssize_t nwriten = 0;
    size_t nleft = size;
    const char *ptr = (const char *) buff;

    while (nleft > 0) {
        nwriten = ::write(fd, ptr, nleft);
        if (nwriten < 0) {
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

    return (size - nleft);
}
