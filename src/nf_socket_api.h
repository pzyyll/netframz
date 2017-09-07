//
// @Create by Caizhili on 20170825
// @Brief socket fd operation
//

#ifndef NF_SRC_SOCKET_API_H
#define NF_SRC_SOCKET_API_H

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

namespace nf {

int SetBlockStat(int fd, bool is_block);

ssize_t Readn(int fd, void *buff, size_t len);

ssize_t Writen(int fd, const void *buff, size_t len);

} //nf

#endif //NF_SRC_SOCKET_API_H
