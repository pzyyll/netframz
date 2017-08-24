//
// @Create by Caizhili On 20170824
// @Brief nf_condition.cpp
//

#include "nf_condition.h"

using namespace nf;

Condition::Condition(Mutex &mutex)
    : cond_(PTHREAD_COND_INITIALIZER),
      mutex_(mutex) {

}

Condition::~Condition() {

}

int Condition::Wait() {
    return pthread_cond_wait(&cond_, &mutex_.GetMutex());
}

int Condition::TimedWait(const unsigned long sec) {
    struct timeval now;
    struct timespec ts;

    gettimeofday(&now, NULL);
    ts.tv_sec = now.tv_sec + sec;
    ts.tv_nsec = now.tv_usec * 1000;

    return pthread_cond_timedwait(&cond_, &mutex_.GetMutex(), &ts);
}

int Condition::Signal() {
    return pthread_cond_signal(&cond_);
}

int Condition::Broadcast() {
    return pthread_cond_broadcast(&cond_);
}
