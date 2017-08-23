//
// @Create by Caizhili on 20170823
// @Brief nf_thread.cpp
//

#include <cassert>

#include "nf_thread.h"

using namespace nf;

Thread::Thread() {
    err_msg_[0] = '\0';
}

Thread::Thread(const ThreadHandle &handle) : handle_(handle) {
    err_msg_[0] = '\0';
}

Thread::~Thread() {

}

int Thread::Run() {
    int err = pthread_create(&tid_, NULL, Thread::ThreadRun, (void *)this);
    if (err != 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Create thread fail. %s", strerror(err));
        return -1;
    }

    return 0;
}

int Thread::Join() {
    int err = pthread_join(tid_, NULL);
    if (err != 0) {
        snprintf(err_msg_, sizeof(err_msg_), "Join thread fail. %s", strerror(err));
        return -1;
    }

    return 0;
}

const char *Thread::GetErrMsg() {
    return err_msg_;
}

void *Thread::ThreadRun(void *args) {
    assert(args != NULL);
    Thread *threadp = (Thread *)args;

    threadp->handle_();

    pthread_exit(NULL);
}
