//
// @Create by Caizhili on 20170906
// @Brief verbose socket api package
//

#include "socket.h"
#include "nf_socket_api.h"

using namespace nf;

Socket::Socket() : sock_(-1) {
    err_msg_[0] = '\0';
}

Socket::Socket(int sock) : sock_(sock) {
    err_msg_[0] = '\0';
}

Socket::Socket(int domain, int type, int protocol) {
    sock_ = socket(domain, type, protocol);
    err_msg_[0] = '\0';
}

Socket::~Socket() {
    //Close();
}

void Socket::SetNonBlock(bool on) {
    SetBlockStat(sock_, !on);
}

void Socket::SetSock(int sock) {
    sock_ = sock;
}

int Socket::GetSock() {
    return sock_;
}

bool Socket::CheckSock() {
    return (sock_ > 0);
}

void Socket::Close() {
    if (sock_ > 0) {
        close(sock_);
        sock_ = -1;
    }
}

const char *Socket::GetErrMsg() {
    return err_msg_;
}

int Socket::GetLocalAddr(Endpoint &ep) {
    struct sockaddr_storage *ssp = ep.GetMutableSA();
    socklen_t len = sizeof(*ssp);
    if (getsockname(sock_, (struct sockaddr *)ssp, &len) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::GetPeerAddr(Endpoint &ep) {
    struct sockaddr_storage *ssp = ep.GetMutableSA();
    socklen_t len = sizeof(*ssp);
    if (getpeername(sock_, (struct sockaddr *)ssp, &len) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::BindAddr(const Endpoint &ep) {
    int ret = SUCCESS;
    switch (ep.GetFamily()) {
    case AF_INET: {
            const struct sockaddr_in *sa_in = ep.GetSAv4();
            ret = bind(sock_, (const struct sockaddr *)sa_in, sizeof(*sa_in));
            if (ret < 0) {
                snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
            }
            break;
        }
    case AF_INET6: {
            const struct sockaddr_in6 *sa_in6 = ep.GetSAv6();
            ret = bind(sock_, (const struct sockaddr *)sa_in6, sizeof(*sa_in6));
            if (ret < 0) {
                snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
            }
            break;
        }
    default:
        ret = FAIL;
        snprintf(err_msg_, sizeof(err_msg_), "Family val is error. %d", ep.GetFamily());
        break;
    }
    return ret;
}

int Socket::Listen(int backlog) {
    int ret = SUCCESS;
    if ( (ret = listen(sock_, backlog)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
    }
    return ret;
}

int Socket::Accept(Endpoint *peerend) {
    int peer_fd = 0;
    if (peerend == NULL) {
        peer_fd = accept(sock_, NULL, NULL);
    } else {
        struct sockaddr_storage *ssp = peerend->GetMutableSA();
        socklen_t ss_len = sizeof(*ssp);
        peer_fd = accept(sock_, (struct sockaddr *)ssp, &ss_len);
    }
    return peer_fd;
}

int Socket::Connect(const Endpoint &ep) {
    const struct sockaddr_storage *ssp = ep.GetSA();
    if (connect(sock_, (const struct sockaddr *)ssp, sizeof(*ssp)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::Connect(const Endpoint &ep, int timeout) {
    const struct sockaddr_storage *ssp = ep.GetSA();

    if (ConnectNonb(sock_, (const struct sockaddr *)ssp,
                sizeof(*ssp), timeout) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

ssize_t Socket::Send(const void *buf, size_t len) {
    ssize_t n = Writen(sock_, buf, len);
    if (n < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
    }
    return n;
}

ssize_t Socket::Recv(void *buf, size_t len) {
    ssize_t n = Readn(sock_, buf, len);
    if (n < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
    }
    return n;
}

int Socket::SetNoDelay(bool on) {
    int optval = on ? 1 : 0;
    if (setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::SetReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::SetReusePort(bool on) {
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
#endif //SO_REUSEPORT

    return SUCCESS;
}

int Socket::SetRecvBuff(int size) {
    if (setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}

int Socket::SetSendBuff(int size) {
    if (setsockopt(sock_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "%s", strerror(errno));
        return FAIL;
    }
    return SUCCESS;
}
