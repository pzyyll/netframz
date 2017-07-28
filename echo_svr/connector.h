//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <functional>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "nf_event_iotask.h"

static const unsigned long DEFAULT_BUFF_SIZE = 1024000;

class ErrCode {
public:
    enum RET {
        FAIL = -1,
        SUCCESS = 0,
    };
public:
    ErrCode(int ret = 0, std::string err_msg = "") : ret_(ret), err_msg_(err_msg) {  }
    ~ErrCode() {  }

    void set_ret(int ret) { ret_ = ret; }
    int get_ret() { return ret_; }

    void set_err_msg(const std::string &err_msg) { err_msg_ = err_msg; }
    std::string get_err_msg() { return err_msg_; }

private:
    int ret_;
    std::string err_msg_;
};

typedef std::function<void(unsigned long, task_data_t, ErrCode)> WRHandler;

struct Buffer {
    explicit Buffer(const unsigned long len)
            : base(new char[len]), lenth(len), fpos(0), tpos(0) {

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

struct StaticBuffer {
    char *buff;
    unsigned long lenth;
};

struct ConnCbData {
    task_data_t pri_data;
    WRHandler handler;
};

class Connector {
public:
    typedef unsigned long size_t;
    typedef long ssize_t;

    enum STAT {
        SUCCESS = 0,
        FAIL = -1,
        BUFF_LIMIT = -1001,
    };

public:
    Connector(EventLoop &loop, int fd);

    virtual ~Connector();

    void BeginRecv(const ConnCbData &cb_data);

    ssize_t Recv(void *buff, const size_t size);

    void Send(const char *buff, const size_t lenth, const ConnCbData &cb_data = ConnCbData());

    void Close();

    bool IsTimeOut(const unsigned long time_limit);

    void SetLastActTimeToNow();

    IOTask &GetIOTask();

    unsigned long GetCID();

    std::string GetErrMsg();

protected:
    Connector(const Connector &);

    Connector &operator=(const Connector &);

    int MakeFdBlockIs(bool is_block, int fd);

    ssize_t InnerRead(void *buff, size_t size);

    ssize_t InnerWrite(const void *buff, const size_t size);

    void OnRead(EventLoop *loopsv, task_data_t data, int mask);

    void OnWriteRemain(EventLoop *loopsv, task_data_t data, int mask);

    int CheckMask(int mask);

    ssize_t Send(const void *buff, const size_t size);

    ssize_t SendRemain();

    size_t GetRemainSize();

protected:
    unsigned long cid_;
    bool is_close_;

    //second
    unsigned long last_act_time_;
    Buffer recv_buf_;
    Buffer send_buf_;
    IOTask *task_;
    ConnCbData rdata, wdata;
    char err_msg_[256];

    static unsigned long id_cnt_;
};

#endif //NF_TEST_CONNETOR_H
