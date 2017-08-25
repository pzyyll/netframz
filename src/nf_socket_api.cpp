//
// @Create by Caizhili on 20170825
// @Brief socket fd operation
//

#include "nf_socket_api.h"

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
