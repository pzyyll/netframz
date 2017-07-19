//
// @ Create by CaiZhili on 2017/7/18
// @ Bref
//
#include <ctime>
#include <sys/time.h>

#include "connector.h"

Connector::Connector(EventLoop &loop, int fd)
    : cid_(id_cnt_++),
      is_close_(0),
      last_act_time_(0),
      recv_buf_(DEFAULT_BUFF_SIZE),
      send_buf_(DEFAULT_BUFF_SIZE),
      task_(new IOTask(loop, fd, 0)) {
    struct timeval now;
    gettimeofday(&now, NULL);
    last_act_time_ = now.tv_sec;
}

Connector::~Connector() {
    if (!is_close_) {
        //todo close connect
        Close();
    }

    if (task_)
        delete task_;
    task_ = NULL;
}

Connector::ssize_t Connector::SyncRead(char *buff, const size_t size) {
    int fd = task_->GetFd();
    MakeFdBlockIs(false, fd);
    ssize_t nbyes = ::read(fd, buff, size);
    if (nbyes < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "fd=%d to read is bad! %s",
                fd, strerror(errno));
        return FAIL;
    }
    return nbyes;
}

Connector::ssize_t Connector::SyncWrite(const char *buff, const size_t size) {
    int fd = task_->GetFd();
    MakeFdBlockIs(true, fd);
    ssize_t nbyes = ::write(fd, buff, size);
    if (nbyes < 0) {
        snprintf(err_msg_, sizeof(err_msg_), "fd(%d) to write is bad! %s",
                fd, strerror(errno));
        return FAIL;
    }
    return nbyes;
}

IOTask& GetIOTask() {
    return *task_;
}
