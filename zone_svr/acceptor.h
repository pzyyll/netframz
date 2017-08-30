//
// @Create by Caizhili on 20170830
// @Brief acceptor.h
//

#ifndef NF_ZONESVR_ACCEPTOR_H
#define NF_ZONESVR_ACCEPTOR_H

#include <functional>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <cassert>

#include "nf_event.h"
#include "nf_event_iotask.h"
#include "nf_socket_api.h"
#include "err_code.h"

class Acceptor {
    static const unsigned int kMaxErrLen = 256;
public:
    typedef std::function<void(int fd, ErrCode &)> CallBack;

    enum Ret {
        FAIL = -1,
        SUCCESS = 0,
    };

public:
    Acceptor();

    Acceptor(EventLoop *loop);

    Acceptor(EventLoop *loop, int listen_fd);

    ~Acceptor();

    int Bind(const std::string &addr, const int port, bool reuse = true);

    int Listen(CallBack cb);

    void SetListenFd(int listen_fd);

    int GetListenFd();

    void SetEventLoop(EventLoop *loop);

    EventLoop *GetEventLoop();

    const char *GetErrMsg();

private:
    void AcceptCb(EventLoop *loop, task_data_t data, int mask);

    int CheckMask(int mask);

private:
    EventLoop *loop_;
    IOTask    *accept_task_;
    int       listen_fd_;
    CallBack  cb_;

    char err_msg_[kMaxErrLen];
};

#endif //NF_ZONESVR_ACCEPTOR_H
