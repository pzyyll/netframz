//
// @Create by Caizhili on 20170906
// @Brief
//

#include "socket.h"

Socket::Socket() : sock_(-1) {
    memset(&sa_, 0, sizeof(sa_));
}

Socket::Socket(int sock) : sock_(sock) {
    memset(&sa_, 0, sizeof(sa_));
}

Socket::~Socket() {

}

void Socket::set_sock(int sock) {
    sock_ = sock;
}

int Socket::sock() {
    return sock_;
}

void Socket::set_sa(const struct sockaddr_in *sa) {
    if (!sa)
        return sa;

    sa_ = *sa;
}
