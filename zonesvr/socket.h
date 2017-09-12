//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_ZONESVR_SOCKET_H
#define NF_ZONESVR_SOCKET_H

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdlib>
#include <cstring>

#include "endpoint.h"

class Socket {
    const static unsigned int kMaxErrLen = 256;
public:
    Socket();

    Socket(int domain, int type, int protocol);

    Socket(int sock);

    ~Socket();

    void SetSock(int sock);

    int GetSock();

    void SetSockaddr(const struct sockaddr *sa, socklen_t sa_len);

    struct sockaddr *GetSockaddr();

    //TODO(@Caizhili)

private:
    int sock_;
    Endpoint ep_;
    char err_msg_;
};

#endif //NF_ZONESVR_SOCKET_H
