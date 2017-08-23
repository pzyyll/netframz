//
// @Create by Caizhili On 20170823
// @Brief nf_mutex.cpp
//

#include "nf_mutex.h"

using namespace nf;

Mutex::Mutex() {
    pthread_mutex_init(&mutex_, NULL);
}

Mutex::~Mutex() {
    pthread_mutex_destroy(&mutex_);
}

void Mutex::Lock() {
    pthread_mutex_lock(&mutex_);
}

int Mutex::TryLock() {
    return pthread_mutex_trylock(&mutex_);
}

void Mutex::Unlock() {
    pthread_mutex_unlock(&mutex_);
}

MutexGuard::MutexGuard(Mutex &mutex) : mutex_(mutex) {
    mutex_.Lock();
}

MutexGuard::~MutexGuard() {
    mutex_.Unlock();
}
