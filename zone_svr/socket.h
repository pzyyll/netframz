//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_ZONESVR_SOCKET_H
#define NF_ZONESVR_SOCKET_H

#include <netinet/in.h>
#include <cstdlib>
#include <cstring>

//TODO

class Socket {
public:
    Socket();

    ~Socket();

private:
    int sock_;
    struct sockaddr_in sa_;
}

#endif //NF_ZONESVR_SOCKET_H

