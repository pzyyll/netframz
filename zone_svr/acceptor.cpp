//
// @Create by Caizhili on 20170830
// @Brief acceptor.h
//

#include "acceptor.h"

#define UNUSED(X) (void)(X)

using namespace std::placeholders;
using namespace nf;

Acceptor::Acceptor()
    : loop_(NULL),
      accept_task_(NULL),
      listen_fd_(-1),
      cb_() {
    err_msg_[0] = '\0';
}

Acceptor::Acceptor(EventLoop *loop)
    : loop_(loop),
      accept_task_(NULL),
      listen_fd_(-1),
      cb_() {
    err_msg_[0] = '\0';
}

Acceptor::Acceptor(EventLoop *loop, int listen_fd)
    : loop_(loop),
      accept_task_(NULL),
      listen_fd_(listen_fd),
      cb_() {
    err_msg_[0] = '\0';
}

Acceptor::~Acceptor() {
    if (accept_task_) {
        delete accept_task_;
        accept_task_ = NULL;
    }
}

int Acceptor::Bind(const std::string &addr, const int port, bool reuse) {
    if (listen_fd_ < 0) {
        if ( (listen_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            snprintf(err_msg_, sizeof(err_msg_), "Create socket fd fail|%s",
                     strerror(errno));
            return FAIL;
        }
    }

    if (reuse) {
        int ireuse = 1;
        if (setsockopt(listen_fd_, SOL_SOCKET,
                       SO_REUSEADDR, &ireuse, sizeof(ireuse)) < 0) {
            snprintf(err_msg_, sizeof(err_msg_), "Set addr reuse fail|%s",
                     strerror(errno));
            return FAIL;
        }
    }

    SetBlockStat(listen_fd_, false);

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    if (addr != "") {
        inet_pton(AF_INET, addr.c_str(), &saddr.sin_addr);
    } else {
        saddr.sin_addr.s_addr = INADDR_ANY;
    }

    if (bind(listen_fd_, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Bind fd(%d) fail. %s",
                 listen_fd_, strerror(errno));
        return FAIL;
    }

    return listen_fd_;
}

int Acceptor::Listen(CallBack cb) {
    cb_ = cb;

    if (listen(listen_fd_, 4096) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Listen fail. fd=%d, %s",
                 listen_fd_, strerror(errno));
        return FAIL;
    }

    if (!loop_) {
        snprintf(err_msg_, sizeof(err_msg_), "EventLoop handler is null.");
        return FAIL;
    }

    accept_task_ = new IOTask(*loop_, listen_fd_, EV_READABLE);
    assert(accept_task_ != NULL);

    accept_task_->Bind(std::bind(&Acceptor::AcceptCb, this, _1, _2, _3));

    if (accept_task_->Start() < 0) {
        snprintf(err_msg_, sizeof(err_msg_),
                 "%s", accept_task_->GetErr().c_str());
        return FAIL;
    }

    return SUCCESS;
}

void Acceptor::SetListenFd(int listen_fd) {
    listen_fd_ = listen_fd;
}

int Acceptor::GetListenFd() {
    return listen_fd_;
}

void Acceptor::SetEventLoop(EventLoop *loop) {
    loop_ = loop;
}

EventLoop *Acceptor::GetEventLoop() {
    return loop_;
}

const char *Acceptor::GetErrMsg() {
    return err_msg_;
}

void Acceptor::AcceptCb(EventLoop *loop, task_data_t data, int mask) {
    UNUSED(loop);
    UNUSED(data);

    ErrCode err(ErrCode::SUCCESS);

    if (CheckMask(mask) < 0) {
        err.set_ret(ErrCode::FAIL);
        err.set_err_msg(err_msg_);
        if (cb_)
            cb_(-1, err);
        return;
    }

    while (true) {
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        socklen_t len = sizeof(cli_addr);
        int fd = accept(listen_fd_, (struct sockaddr *) &cli_addr, &len);
        if (fd < 0) {
            if (EAGAIN == errno)
                break;
            err.set_ret(ErrCode::FAIL);
            err.set_err_msg("Accept err.");
            if (cb_)
                cb_(-1, err);
            return;
        }

        if (cb_)
            cb_(fd, err);
    }
}

int Acceptor::CheckMask(int mask) {
    if (mask & EV_RDHUP) {
        snprintf(err_msg_, sizeof(err_msg_), "already closed by foreign.");
        return FAIL;
    }

    if (mask & EV_HUP) {
        snprintf(err_msg_, sizeof(err_msg_), "close cause hup.");
        return FAIL;
    }

    if (mask & EV_ERR) {
        snprintf(err_msg_, sizeof(err_msg_), "happen err.");
        return FAIL;
    }

    return SUCCESS;
}
