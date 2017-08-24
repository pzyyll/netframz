//
// @Create by Caizhili on 20170824
// @Brief nf_condition.h
//

#include <pthread.h>
#include <sys/time.h>
#include <ctime>

#include "nf_mutex.h"

namespace nf {

class Condition {
public:
    Condition(Mutex &mutex);

    ~Condition();

    int Wait();

    int TimedWait(const unsigned long sec);

    int Signal();

    int Broadcast();

private:
    pthread_cond_t cond_;
    Mutex &mutex_;
};

}
