//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#ifndef NF_ZONESVR_ENDPOINT_H
#define NF_ZONESVR_ENDPOINT_H

#include <string>
#include <cstring>
#include <cstddef>
#include <netinet/in.h>
#include <arpa/inet.h>

class Endpoint {
public:
    const static size_t kMaxIPStrLen = 128;
    typedef struct sockaddr_storage SAStorage;

    Endpoint();

    Endpoint(int port, const std::string &addr = "", bool ipv6 = false);

    ~Endpoint();

    void SetSockAddr(const struct sockaddr *sa, socklen_t sa_len);

    void GetIPStr(std::string &str);

    //TODO(@Caizhili)

    int GetPort();

    void GetHost(std::string &str);

private:
    SAStorage sa_;
};

#endif //NF_ZONESVR_ENDPOINT_H
