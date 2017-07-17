//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <sys/time.h>
#include "../src/nf_event_iotask.h"

struct Buffer {
    explicit Buffer(unsigned long size)
        : m_base(new char[size]), m_size(size), m_npos(m_base) {

    }

    ~Buffer() {
        if (m_base)
            delete[] m_base;
        m_base = NULL;
    }

    char *m_base;
    unsigned long m_size;
    char *m_npos;
}

class Connetor {
public:
    Connetor();
    virtual ~Connetor();

    int OnTimeOut(const unsigned long time_limit, const struct timeval &now);

private:
    //second
    unsigned long last_act_time_;
    IOTask *task_;
};

#endif //NF_TEST_CONNETOR_H


