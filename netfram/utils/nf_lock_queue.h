//
// @Create by Caizhili on 20170906
// @Brief
//

#ifndef NF_SRC_LOCK_QUEUE_H
#define NF_SRC_LOCK_QUEUE_H

#include <deque>
#include "nf_mutex.h"

namespace nf {

template <typename T>
class LockQueue {
public:
    LockQueue() {  }

    ~LockQueue() {  }

    void Push(const T &item) {
        LockGuard lock(mutex_);
        queue_.push_back(item);
    }

    bool Next(T &result) {
        LockGuard lock(mutex_);

        if (queue_.empty())
            return false;

        result = queue_.front();
        queue_.pop_front();

        return true;
    }

    bool Empty() {
        LockGuard lock(mutex_);
        return queue_.empty();
    }

    void Lock() {
        mutex_.Lock();
    }

    void Unlock() {
        mutex_.Unlock();
    }

private:
    Mutex mutex_;

    std::deque<T> queue_;
};

} //nf
#endif // NF_SRC_LOCK_QUEUE_H
