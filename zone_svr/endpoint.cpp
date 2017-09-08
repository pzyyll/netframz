//
// @Create by Caizhili on 20170907
// @Brief wrap ip addr info
//

#include "endpoint.h"

//TODO(@Caizhili)

/*
 * @Brief struct sockaddr_in detail for IPv4.
 * struct sockaddr_in {
 *     sa_family_t    sin_family;
 *     in_port_t      sin_port;
 *     struct in_addr sin_addr;
 * }
 *
 * struct in_addr {
 *     uint32_t s_addr;
 * }
 */
void SAv4From(int port, const char *addr, struct sockaddr_in *sa) {
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);

    if (addr == NULL || strlen(addr) == 0)
        sa->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, addr, &sa->sin_addr);
}

/*
 * @Brief struct sockaddr_in6 detail for IPv6.
 * struct sockaddr_in6 {
 *     sa_family_t     sin6_family;
 *     in_port_t       sin6_port;
 *     uint32_t        sin6_flowinfor;  // < TODO understand
 *     struct in6_addr sin6_addr;
 *     uint32_t        sin6_scope_id;   // < TODO understand
 * }
 *
 * struct in6_addr {
 *     unsigned char s6_addr[16];
 * }
 */
void SAv6From(int port, const char *addr, struct sockaddr_in6 *sa6) {
    sa6->sin6_family = AF_INET6;
    sa6->sin6_port = htons(port);

    if (addr == NULL || strlen(addr) == 0)
        sa6->sin6_addr = in6addr_any;
    else
        inet_pton(AF_INET6, addr, &sa6->sin6_addr);
}

/*
 * @Brief Convert IP presentation from sockaddr_xxx struct;
 */
void SAToIPStr(const struct sockaddr *sa, socklen_t sa_len,
               char *ip_str, size_t len) {
    char portstr[8];
    size_t rlen = 0;

    switch (sa->sa_family) {
    case AF_INET: {
            if ((size_t)sa_len < sizeof(struct sockaddr_in)
                    || len < INET_ADDRSTRLEN) {
                return;
            }
            const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sa;
            if (!inet_ntop(AF_INET, &sa_in->sin_addr, ip_str, (socklen_t)len)) {
                return;
            }
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sa_in->sin_port));
            rlen = len - INET_ADDRSTRLEN;
            break;
        }
    case AF_INET6: {
            if ((size_t)sa_len < sizeof(struct sockaddr_in6)
                    || len < INET6_ADDRSTRLEN) {
                return;
            }
            const struct sockaddr_in6 *sa_i6 = (const struct sockaddr_in6 *)sa;
            if (!inet_ntop(AF_INET6, &sa_i6->sin6_addr, ip_str, (socklen_t)len)) {
                return;
            }
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sa_i6->sin6_port));
            rlen = len - INET6_ADDRSTRLEN;
            break;
        }
    default:
        return;
    }

    strncat(ip_str, portstr, rlen > 0 ? rlen - 1 : 0);
}

void SAToHost(const struct sockaddr *sa, socklen_t sa_len,
              char *host, size_t len) {
    switch (sa->sa_family) {
    case AF_INET: {
            if ((size_t)sa_len < sizeof(struct sockaddr_in)
                    || len < INET_ADDRSTRLEN) {
                return;
            }
            const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sa;
            inet_ntop(AF_INET, &sa_in->sin_addr, host, (socklen_t)len);
            break;
        }
    case AF_INET6: {
            if ((size_t)sa_len < sizeof(struct sockaddr_in6)
                    || len < INET6_ADDRSTRLEN) {
                return;
            }
            const struct sockaddr_in6 *sa_i6 = (const struct sockaddr_in6 *)sa;
            inet_ntop(AF_INET6, &sa_i6->sin6_addr, host, (socklen_t)len);
            break;
        }
    default:
        break;
    }
}

int SAToPort(const struct sockaddr *sa, socklen_t sa_len) {
    int port = 0;

    switch (sa->sa_family) {
    case AF_INET: {
            if (sizeof(struct sockaddr_in) > (size_t)sa_len)
                return 0;
            const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sa;
            port = ntohs(sa_in->sin_port);
            break;
        }
    case AF_INET6: {
            if (sizeof(struct sockaddr_in6) > (size_t)sa_len)
                return 0;
            const struct sockaddr_in6 *sa_i6 = (const struct sockaddr_in6 *)sa;
            port = ntohs(sa_i6->sin6_port);
            break;
        }
    default:
        break;
    }

    return port;
}

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

void Endpoint::SetSockAddr(const struct sockaddr *sa, socklen_t sa_len) {
    if (!sa || sa_len == 0)
        return;

    size_t cp_len = sizeof(sa) > sa_len ? sa_len : sizeof(sa);
    memcpy(&sa_, sa, cp_len);
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
