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
#include "err_code.h"

static const unsigned long DEFAULT_BUFF_SIZE = 1024 * 1024;

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

    //以下提供的几个接口只是方便操作，调用前需要自行检查越界问题。
    unsigned long MaxSize() { return lenth; }
    unsigned long UsedSize() { return (unsigned long)(tpos - fpos); }
    unsigned long RemainSize() { return (unsigned long)(lenth - tpos); }
    void FrontAdvancing(long step_size) {
        fpos += step_size;
        if (fpos == tpos)
            fpos = tpos = 0;
    }
    void TailAdvancing(long step_size) { tpos += step_size; }
    char *FrontPos() { return (base + fpos); }
    char *TailPos() { return (base + tpos); }
    void MemoryMove2Left() {
        if (0 == fpos)
            return;
        memmove(base, base + fpos, tpos - fpos);
        tpos -= fpos;
        fpos = 0;
    }

    char *base;
    unsigned long lenth;
    unsigned long fpos, tpos;
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

    size_t Recv(void *buff, const size_t size);

    void Send(const char *buff, const size_t lenth, const ConnCbData &cb_data = ConnCbData());

    void Close();

    bool IsTimeOut(const unsigned long time_limit);

    void SetLastActTimeToNow();

    IOTask &GetIOTask();

    unsigned long GetCID();

    std::string GetErrMsg();

    size_t GetRvBuffLenth();

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
