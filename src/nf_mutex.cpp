//
// @Create by Caizhili On 20170823
// @Brief nf_mutex.cpp
//

#include "nf_mutex.h"

using namespace nf;

Mutex::Mutex(): mutex_(PTHREAD_MUTEX_INITIALIZER) {

}

Mutex::~Mutex() {

}

int Mutex::Lock() {
    return pthread_mutex_lock(&mutex_);
}

int Mutex::TryLock() {
    return pthread_mutex_trylock(&mutex_);
}

int Mutex::Unlock() {
    return pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t &Mutex::GetMutex() {
    return mutex_;
}

MutexGuard::MutexGuard(Mutex &mutex) : mutex_(mutex) {
    mutex_.Lock();
}

MutexGuard::~MutexGuard() {
    mutex_.Unlock();
}
