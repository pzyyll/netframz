//
// @ Create by CaiZhili on 2017/7/17
// @ Bref
//
#ifndef NF_TEST_CONNETOR_H
#define NF_TEST_CONNETOR_H

#include <functional>
#include <vector>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include "nf_event_iotask.h"
#include "nf_mutex.h"
#include "err_code.h"
#include "socket.h"

static const unsigned long DEFAULT_BUFF_SIZE = 1024 * 1024;

typedef std::function<void(unsigned long, task_data_t, ErrCode)> WRHandler;

struct Buffer {
    Buffer() : fpos(0), tpos(0) {

    }

    Buffer(const unsigned long len)
        : fpos(0), tpos(0) {
        storage.reserve(len);
    }

    ~Buffer() {
    }

    unsigned long MaxSize() { return storage.capacity(); }
    unsigned long UsedSize() { return (unsigned long)(tpos - fpos); }
    unsigned long RemainSize() { return (unsigned long)(MaxSize() - tpos); }
    int Resize(unsigned long size) {
        if (size > MaxSize())
            storage.reserve(size);
        return 0;
    }
    void FrontAdvancing(unsigned long step_size) {
        if (fpos + step_size > tpos)
            return;
        fpos += step_size;
        if (fpos == tpos)
            fpos = tpos = 0;
    }
    void TailAdvancing(unsigned long step_size) {
        if (tpos + step_size > MaxSize())
            return;
        tpos += step_size;
    }
    char *FrontPos() { return (&storage[0] + fpos); }
    char *TailPos() { return (&storage[0] + tpos); }
    void MemoryMove2Left() {
        if (0 == fpos)
            return;
        memmove(&storage[0], &storage[0] + fpos, tpos - fpos);
        tpos -= fpos;
        fpos = 0;
    }

    unsigned long fpos, tpos;
    std::vector<char> storage;
};

struct ConnCbData {
    task_data_t pri_data;
    WRHandler   handler;
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
    Connector(EventService &es, int fd);

    virtual ~Connector();

    void Open(const ConnCbData &cb_data = ConnCbData());

    size_t Recv(void *buff, const size_t size);

    void Send(const char *buff,
              const size_t lenth,
              const ConnCbData &cb_data = ConnCbData());

    void Close();

    bool IsTimeOut(const unsigned long time_limit);

    bool IsClose();

    void SetLastActTimeToNow();

    struct Buffer &GetRecvBuff();

    unsigned long GetCID();

    Socket &GetSocket();

    std::string GetErrMsg();

protected:
    virtual void HandleInput(ErrCode &err_code);

    virtual void HandleOutput(ErrCode &err_code);

protected:
    Connector(const Connector &);

    Connector &operator=(const Connector &);

    void OnRead(EventService *es, task_data_t data, int mask);

    void OnWriteRemain(EventService *es, task_data_t data, int mask);

    int CheckMask(int mask);

    ssize_t Send(const void *buff, const size_t size);

    ssize_t SendRemain();

protected:
    unsigned long  cid_;
    bool           is_close_;

    //second
    unsigned long  last_act_time_;
    Buffer         recv_buf_;
    Buffer         send_buf_;
    IOTask         *task_;
    ConnCbData     rdata_, wdata_;
    char           err_msg_[256];

    Socket         socket_;

    static unsigned long id_cnt_;
    static unsigned long GetIdCnt();
};

#endif //NF_TEST_CONNETOR_H
