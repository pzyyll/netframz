//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_ZONESVR_SOCKET_H
#define NF_ZONESVR_SOCKET_H

#include <netinet/in.h>
#include <cstdlib>
#include <cstring>

class Socket {
public:
    Socket();

    Socket(int sock);

    ~Socket();

    void set_sock(int sock);

    int sock();

    void set_sa(const struct sockaddr_in *sa);

    struct sockaddr_in *sa();

private:
    int sock_;
    struct sockaddr_in sa_;
};

#endif //NF_ZONESVR_SOCKET_H
