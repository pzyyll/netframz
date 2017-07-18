//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <functional>
#include <sys/time.h>

#include "../src/nf_event_iotask.h"

struct Buffer {
    explicit Buffer(unsigned long size)
        : m_base(new char[size]), m_size(size), m_fpos(0), m_tpos(0) {

    }

    ~Buffer() {
        if (m_base)
            delete[] m_base;
        m_base = NULL;
    }

    char *m_base;
    unsigned long m_size;
    unsigned long m_fpos, m_tpos;
};

class Connector {
public:
    typedef unsigned long size_t;
    typedef std::function<void (EventLoop *, task_data_t, int)> handle_t;

public:
    Connector(EventLoop &loop, int fd, handle_t w, handle_t r);
    virtual ~Connector();

    int OnTimeOut(const unsigned long time_limit, const struct timeval &now);
    size_t Write(const char *buff, const size_t size);
    size_t Read(char *buff, const size_t size);

    size_t WriteRemain();

    IOTask& GetIOTask();

    //close the connect
    void Close();

protected:
    Connector(const Connector&);
    Connector& operator=(const Connector&);

private:
    bool is_close_;

    //second
    unsigned long last_act_time_;
    Buffer recv_buf_;
    Buffer send_buf_;
    IOTask *task_;
    handle_t w, r;
};

#endif //NF_TEST_CONNETOR_H
