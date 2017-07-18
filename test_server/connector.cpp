//
// @ Create by CaiZhili on 2017/7/18
// @ Bref
//
#include <ctime>
#include <sys/time.h>
#include "connector.h"

Connector::Connector(unsigned long uid, EventLoop &loop, int fd)
    : uid_(uid),
      is_close_(0),
      last_act_time_(0),
      recv_buf_(DEFAULT_BUFF_SIZE),
      send_buf_(DEFAULT_BUFF_SIZE),
      task_(new IOTask(loop, fd, 0)){
    struct timeval now;
    gettimeofday(&now, NULL);
    last_act_time_ = now.tv_sec;
}

Connector::Connector() {
    if (!is_close_) {
        //tode close connect
        Close();
    }

    if (task_)
        delete task_;
    task_ = NULL;
}

int Write(const char *buff, const size_t size) {
    ssize_t nwriten = 0;
    ssize_t nleft = size;

    while (nleft > 0) {
        //todo first
        write()
    }

}

IOTask& GetIOTask() {
    return *task_;
}
