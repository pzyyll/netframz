//
// @Create by Caizhili on 20170825
// @Brief socket fd operation
//

#include "nf_socket_api.h"

namespace nf {

int SetBlockStat(int fd, bool is_block) {
    int val;

    if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
        return -1;

    if (is_block) {
        val &= ~O_NONBLOCK;
    } else {
        val |= O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, val) < 0)
        return -1;

    return 0;
}

int ConnectNonb(int sockfd, const struct sockaddr *sa, socklen_t salen, int timeout) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0)
        return -1;

    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int err = 0;

    do {
        int n = 0;
        if ( (n = connect(sockfd, sa, salen)) < 0)
            if (errno != EINPROGRESS)
                return -1;
        if (n == 0)
            break;

        fd_set rset, wset;
        FD_ZERO(&rset);
        FD_SET(sockfd, &rset);
        wset = rset;
        struct timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = timeout * 1000;
        if ( (n = select(sockfd + 1, &rset, &wset, NULL,
                         timeout < 0? NULL: &tv)) == 0) {
            close(sockfd);
            errno = ETIMEDOUT;
            return -1;
        }

        if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
            socklen_t len = sizeof(err);
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
                return -1;
        }

    } while (false);

    fcntl(sockfd, F_SETFL, flags);
    if (err) {
        close(sockfd);
        errno = err;
        return -1;
    }
    return 0;
}

ssize_t Readn(int fd, void *buff, size_t len) {
    ssize_t rn = 0;
    size_t nleft = len;
    char *ptr = (char *)buff;

    while (nleft > 0) {
        rn = read(fd, ptr, nleft);
        if (rn < 0) {
            if (EINTR == errno)
                continue;
            if (EAGAIN == errno)
                break;
            return -1;
        } else if (rn == 0) {
            break;
        }
        ptr += rn;
        nleft -= rn;
    }

    return (ssize_t)(len - nleft);
}

ssize_t Writen(int fd, const void *buff, size_t len) {
    ssize_t wn = 0;
    size_t nleft = len;
    const char *ptr = (const char *)buff;

    while (nleft > 0) {
        wn = write(fd, ptr, nleft);
        if (wn <= 0) {
            if (EINTR == errno)
                continue;
            if (EAGAIN == errno)
                break;
            return -1;
        }
        ptr += wn;
        nleft -= wn;
    }

    return (ssize_t)(len - nleft);
}

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

void GetIPFromHost(const std::string &host, int domain, std::vector<std::string> &v_ip) {
    /* @Brief
     * struct addrinfo {
     *   int             ai_flags;
     *   int             ai_family;
     *   int             ai_socktype;
     *   int             ai_protocol;
     *   socklen_t       ai_addrlen;
     *   char            *ai_canonname;
     *   struct sockaddr *ai_addr;
     *   struct addrinfo *ai_next;
     * }
     */
    struct addrinfo hints;
    struct addrinfo *result = NULL, *rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = domain;

    /* @Brief
     * getaddrinfo(const char *node,
     *             const char *service,
     *             const struct addrinfo *hints,
     *             struct addrinfo **res);
     */
    int ret = getaddrinfo(host.c_str(), 0, &hints, &result);
    if (ret != 0) {
        //cout << gai_strerror(ret) << endl;
        return;
    }

    char ipstr[48] = {0};
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        switch (rp->ai_family) {
        case AF_INET: {
                struct sockaddr_in *sa_in = (struct sockaddr_in *)rp->ai_addr;
                if (inet_ntop(AF_INET,
                              &sa_in->sin_addr,
                              ipstr,
                              sizeof(ipstr)) == NULL) {
                    //cout << strerror(ret) << endl;
                    return ;
                }
                if (AF_INET == domain)
                    v_ip.push_back(ipstr);
                break;
            }
        case AF_INET6: {
                struct sockaddr_in6 *sa_in6 = (struct sockaddr_in6 *)rp->ai_addr;
                if (inet_ntop(AF_INET6,
                              &sa_in6->sin6_addr,
                              ipstr,
                              sizeof(ipstr)) == NULL) {
                    //cout << strerror(ret) << endl;
                    return;
                }
                if (AF_INET6 == domain)
                    v_ip.push_back(ipstr);
                break;
            }
        default:
            break;
        }
    }
}

} //nf
