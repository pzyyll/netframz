//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_IOTASK_H
#define NETFRAMZ_NF_EVENT_IOTASK_H

#include <functional>
#include "nf_event_config.h"

class EventLoop;

class IOTaskImpl;

class IOTask {
public:
    typedef std::function<void(EventLoop *, task_data_t data, int)> handle_t;
    typedef IOTaskImpl impl_type;

public:
    IOTask(EventLoop &loop, const int fd);

    IOTask(EventLoop &loop, const int fd, const int mask);

    IOTask(EventLoop &loop, const int fd, const int mask, handle_t op);

    ~IOTask();

    void Bind(handle_t handle);

    void Start();

    void Restart();

    void Stop();

    void SetPrivateData(task_data_t data);

    task_data_t GetPrivateData();

    void SetMask(int mask);

    int GetMask();

    int GetFd();

private:
    EventLoop &loop_;
    impl_type *pimpl_;
};

#endif //NETFRAMZ_NF_EVENT_IOTASK_H
