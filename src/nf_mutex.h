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

    void Lock();

    int TryLock();

    void Unlock();

private:
    pthread_mutex_t mutex_;
};

class MutexGuard {
public:
    explicit MutexGuard(Mutex &mutex);

    ~MutexGuard();

private:
    Mutex &mutex_;
};

}

#endif //NF_SRC_MUTEX_H
