//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#include "endpoint.h"
#include "nf_socket_api.h"

using namespace nf;

Endpoint::Endpoint() {
    memset(&sa_, 0, sizeof(sa_));
}

Endpoint::Endpoint(int port, const std::string &addr, bool ipv6) {
    memset(&sa_, 0, sizeof(sa_));
    if (ipv6) {
        SAv6From(port, addr.c_str(), (struct sockaddr_in6 *)&sa_);
    } else {
        SAv4From(port, addr.c_str(), (struct sockaddr_in *)&sa_);
    }
}

Endpoint::~Endpoint() {

}

void Endpoint::SetSockaddr(const struct sockaddr *sa, socklen_t sa_len) {
    if (!sa || sa_len == 0 || sa_len > sizeof(sa_))
        return;

    memcpy(&sa_, sa, sa_len);
}

void Endpoint::GetIPStr(std::string &str) {
    char cstr[kMaxIPStrLen] = {0};
    SAToIPStr((struct sockaddr *)&sa_, sizeof(sa_), cstr, sizeof(cstr));
    str.assign(cstr);
}

int Endpoint::GetPort() {
    return SAToPort((struct sockaddr *)&sa_, sizeof(sa_));
}

void Endpoint::GetHost(std::string &str) {
    char cstr[kMaxIPStrLen] = {0};
    SAToHost((struct sockaddr *)&sa_, sizeof(sa_), cstr, sizeof(cstr));
    str.assign(cstr);
}

int Endpoint::GetFamily() {
    return sa_.ss_family;
}

const struct sockaddr_storage *Endpoint::GetSA() {
    return GetMutableSA();
}

struct sockaddr_storage *Endpoint::GetMutableSA() {
    return &sa_;
}

const struct sockaddr_in *Endpoint::GetSAv4() {
    return (const struct sockaddr_in *)&sa_;
}

const struct sockaddr_in6 *Endpoint::GetSAv6() {
    return (const struct sockaddr_in6 *)&sa_;
}
