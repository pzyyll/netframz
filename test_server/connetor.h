//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <sys/time.h>
#include "../src/nf_event_iotask.h"

class Connetor {
public:
    Connetor();
    ~Connetor();

    int OnTimeOut(const unsigned long time_limit, const struct timeval &now);

private:
    //second
    unsigned long last_act_time_;
    IOTask *task;
};

#endif //NF_TEST_CONNETOR_H


