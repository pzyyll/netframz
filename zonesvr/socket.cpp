//
// @Create by Caizhili on 20170906
// @Brief
//

#include "socket.h"

Socket::Socket() : sock_(-1) {
    memset(&sa_, 0, sizeof(sa_));
}

Socket::Socket(int domain, int type, int protocol) {
    sock_ = socket(domain, type, protocol);
}

Socket::Socket(int sock) : sock_(sock) {
    memset(&sa_, 0, sizeof(sa_));
}

Socket::~Socket() {

}

void Socket::SetSock(int sock) {
    sock_ = sock;
}

int Socket::GetSock() {
    return sock_;
}

void Socket::SetSockaddr(const struct sockaddr *sa, socklen_t sa_len) {
    ep_.SetSockaddr(sa, sa_len);
}
