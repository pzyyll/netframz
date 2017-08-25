//
// @Create by Caizhili On 20170823
// @Brief nf_mutex.h
//

#ifndef NF_SRC_MUTEX_H
#define NF_SRC_MUTEX_H

#include <unistd.h>
#include <pthread.h>

namespace nf {

class Mutex {
public:
    Mutex();

    ~Mutex();

    int Lock();

    int TryLock();

    int Unlock();

    pthread_mutex_t &GetMutex();

private:
    pthread_mutex_t mutex_;
};

class LockGuard {
public:
    explicit LockGuard(Mutex &mutex);

    ~LockGuard();

    int Lock();

    int TryLock();

    int Unlock();

private:
    Mutex &mutex_;
};

}

#endif //NF_SRC_MUTEX_H
