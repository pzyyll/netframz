//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#ifndef NF_ZONESVR_ENDPOINT_H
#define NF_ZONESVR_ENDPOINT_H

#include <string>
#include <cstring>
#include <netinet/in.h>


class Endpoint {
public:
    typedef struct sockaddr_storage SAStorage;
    typedef struct sockaddr_in SAv4;
    typedef struct sockaddr_in6 SAv6;
    typedef struct sockaddr SA;

    Endpoint(int port, const std::string addr = "", bool ipv6 = false);

    ~Endpoint();

    //TODO(@Caizhili)

private:
    SAStorage sa_;
}

#endif //NF_ZONESVR_ENDPOINT_H
