//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <functional>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

#include "../src/nf_event_iotask.h"

static const unsigned long DEFAULT_BUFF_SIZE = 1024000;

struct Buffer {
    explicit Buffer(const unsigned long len)
        : base(new char[size]), lenth(len), fpos(0), tpos(0) {

    }

    ~Buffer() {
        if (base)
            delete[] base;
        base = NULL;
    }

    char *base;
    unsigned long lenth;
    unsigned long fpos, tpos;
};

class Connector {
public:
    typedef unsigned long size_t;
    typedef long ssize_t;

    enum STAT {
        SUCCESS = 0;
        FAIL = -1;
        BUFF_LIMIT = -1001,
    };

public:
    Connector(EventLoop &loop, int fd);
    virtual ~Connector();

    ssize_t Read(char *buff, const size_t size);
    ssize_t Write(const char *buff, const size_t size);

    ssize_t WriteRemain();
    //size_t GetRemainSize();

    //close the connect
    void Close();
    int OnTimeOut(const unsigned long time_limit, const struct timeval &now);

    IOTask& GetIOTask();
    unsigned long GetCID();

protected:
    Connector(const Connector&);
    Connector& operator=(const Connector&);

    int MakeFdBlockIs(bool is_block, int fd);
    ssize_t InnerRead(void *buff, size_t size);
    ssize_t InnerWrite(const void *buff, const size_t size);

protected:
    unsigned long cid_;
    bool is_close_;

    //second
    unsigned long last_act_time_;
    Buffer recv_buf_;
    Buffer send_buf_;
    IOTask *task_;
    char err_msg_[256];

    static unsigned long id_cnt_;
};

unsigned long Connector::id_cnt_ = 1;

#endif //NF_TEST_CONNETOR_H
