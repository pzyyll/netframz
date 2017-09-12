//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#ifndef NF_ZONESVR_ENDPOINT_H
#define NF_ZONESVR_ENDPOINT_H

#include <string>
#include <cstring>
#include <cstddef>
#include <cstdio>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Endpoint {
public:
    const static size_t kMaxIPStrLen = 128;
    typedef struct sockaddr_storage SAStorage;

    Endpoint();

    Endpoint(int port, const std::string &addr = "", bool ipv6 = false);

    ~Endpoint();

    void SetSockaddr(const struct sockaddr *sa, socklen_t sa_len);

    std::string GetIP() const;

    //TODO(@Caizhili)

    int GetPort() const;

    std::string GetHost() const;

    int GetFamily() const;

    /*
     * @Brief
     * struct sockaddr_storage {
     *   sa_family_t sa_family;
     *   char        sa_data[]; //variable-length data;
     * }
     */
    const struct sockaddr_storage *GetSA() const;

    struct sockaddr_storage *GetMutableSA();

    const struct sockaddr_in *GetSAv4() const;

    const struct sockaddr_in6 *GetSAv6() const;

private:
    SAStorage sa_;
};

#endif //NF_ZONESVR_ENDPOINT_H
