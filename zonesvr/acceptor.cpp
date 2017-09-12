//
// @Create by Caizhili on 20170830
// @Brief acceptor.h
//

#include "acceptor.h"
#include <iostream>

#define UNUSED(X) (void)(X)

using namespace std::placeholders;
using namespace nf;

Acceptor::Acceptor()
    : es_(NULL),
      accept_task_(NULL),
      listen_(AF_INET, SOCK_STREAM, 0),
      cb_() {
    err_msg_[0] = '\0';
}

Acceptor::Acceptor(EventService *es)
    : es_(es),
      accept_task_(NULL),
      listen_(AF_INET, SOCK_STREAM, 0),
      cb_() {
    err_msg_[0] = '\0';
}

Acceptor::Acceptor(EventService *es, int listen_fd)
    : es_(es),
      accept_task_(NULL),
      listen_(listen_fd),
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
    if (listen_.BindAddr(Endpoint(port, addr)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Bind addr err. %s", listen_.GetErrMsg());
        return FAIL;
    }

    if (listen_.SetReuseAddr(reuse) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Set reuse fail. %s", listen_.GetErrMsg());
        return FAIL;
    }

    //TODO (@Caizhili) test
    listen_.SetNonBlock(true);

     return SUCCESS;
}

int Acceptor::Listen(CallBack cb) {
    cb_ = cb;

    if (!es_) {
        snprintf(err_msg_, sizeof(err_msg_), "EventService handler is null.");
        return FAIL;
    }

    if (listen_.Listen(4096) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Listen fail. fd=%d, %s",
                 listen_.GetSock(), listen_.GetErrMsg());
        return FAIL;
    }

    accept_task_ = new IOTask(*es_, listen_.GetSock(), EV_READABLE);
    assert(accept_task_ != NULL);

    accept_task_->Bind(std::bind(&Acceptor::AcceptCb, this, _1, _2, _3));

    if (accept_task_->Start() < 0) {
        snprintf(err_msg_, sizeof(err_msg_),
                 "%s", accept_task_->GetErr().c_str());
        return FAIL;
    }

    return SUCCESS;
}

/*
 * @Brief
 * If had bind and listen, need rebind and restart listen;
 */
void Acceptor::SetListenFd(int listen_fd) {
    listen_.SetSock(listen_fd);
}

int Acceptor::GetListenFd() {
    return listen_.GetSock();
}

void Acceptor::SetEventService(EventService *es) {
    es_ = es;
}

EventService *Acceptor::GetEventService() {
    return es_;
}

const char *Acceptor::GetErrMsg() {
    return err_msg_;
}

void Acceptor::AcceptCb(EventService *es, task_data_t data, int mask) {
    UNUSED(es);
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
        Endpoint cli_ep;
        int fd = listen_.Accept(&cli_ep);
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
