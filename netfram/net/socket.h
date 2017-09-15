//
// @Create by Caizhili on 20170906
// @Brief verbose socket api package
//

#ifndef NF_ZONESVR_SOCKET_H
#define NF_ZONESVR_SOCKET_H

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#include "endpoint.h"

class Socket {
    const static unsigned int kMaxErrLen = 256;
public:
    enum Ret {
        FAIL = -1,
        SUCCESS = 0,
    };

    Socket();

    Socket(int sock);

    Socket(int domain, int type, int protocol);

    ~Socket();

    void SetNonBlock(bool on);

    void SetSock(int sock);

    int GetSock();

    bool CheckSock();

    void Close();

    const char *GetErrMsg();

    //TODO(@Caizhili) test all interface

    /*
     * @Brief
     * Get local sockaddr info from socket fd;
     */
    int GetLocalAddr(Endpoint &ep);

    /*
     * @Brief
     * Get peer end sockaddr info from socket fd;
     */
    int GetPeerAddr(Endpoint &ep);

    int BindAddr(const Endpoint &ep);

    int Listen(int backlog);

    int Accept(Endpoint *peerend = NULL);

    int Connect(const Endpoint &ep);

    int Connect(const Endpoint &ep, int timeout);

    ssize_t Send(const void *buf, size_t len);

    ssize_t Recv(void *buf, size_t len);

    int SetNoDelay(bool on);

    int SetReuseAddr(bool on);

    /*
     * @Linux 3.9 later
     * Allow mutil process bind same port.
     */
    int SetReusePort(bool on);

    /*
     * @Brief
     * Set tcp recv buff
     * :/proc/sys/net/core/wmem_max
     * The real val is 2x set size;
     */
    int SetRecvBuff(int size);

    int SetSendBuff(int size);

private:
    int sock_;
    char err_msg_[kMaxErrLen];
};

#endif //NF_ZONESVR_SOCKET_H
