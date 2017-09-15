//
// @Create by Caizhili on 20170825
// @Brief socket fd operation
//

#ifndef NF_SRC_SOCKET_API_H
#define NF_SRC_SOCKET_API_H

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

namespace nf {

int SetBlockStat(int fd, bool is_block);

int ConnectNonb(int sockfd, const struct sockaddr *sa, socklen_t salen, int timeout);

ssize_t Readn(int fd, void *buff, size_t len);

ssize_t Writen(int fd, const void *buff, size_t len);

void SAv4From(int port, const char *addr, struct sockaddr_in *sa);

void SAv6From(int port, const char *addr, struct sockaddr_in6 *sa6);

/*
 * @Brief
 * Get IP presentation from sockaddr_xxx
 * Only IPv4 and IPv6.
 * e.g. xxxx.xxxx.xxxx.xxxx:port
 *      xxxx:xxxx:xxxx:xxxx:port
 */
void SAToIPStr(const struct sockaddr *sa, socklen_t sa_len,
               char *ip_str, size_t len);

/*
 * @Brief
 * Get IP presentation not port
 */
void SAToHost(const struct sockaddr *sa, socklen_t sa_len,
              char *host, size_t len);

int SAToPort(const struct sockaddr *sa, socklen_t sa_len);

/*
 * @Brief
 * Get IP from host name;
 * e.g. pzyyll.top -> 172.x.x.x
 */
void GetIPFromHost(const std::string &host, int domain, std::vector<std::string> &v_ip);

} //nf

#endif //NF_SRC_SOCKET_API_H
