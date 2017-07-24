//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_IOTASKIMPL_H
#define NETFRAMZ_NF_EVENT_IOTASKIMPL_H

#include <memory>
#include <functional>
#include "nf_event_config.h"

class EventLoop;

class IOTaskImpl;

class IOTaskImpl {
public:
    typedef std::function<void(EventLoop *, task_data_t data, int)> handle_t;

public:
    IOTaskImpl();

    IOTaskImpl(const int fd, const int mask);

    IOTaskImpl(const int fd, const int mask, handle_t op);

    ~IOTaskImpl();

    void Bind(handle_t handle);

    void Process(EventLoop *loop, int mask);

    void set_fd(int fd);

    int get_fd();

    void set_mask(int mask);

    int get_mask();

    void set_data(task_data_t data);

    task_data_t get_data();

private:
    int fd_;
    int mask_;
    handle_t op_;
    task_data_t data_;
};

#endif //NETFRAMZ_NF_EVENT_IOTASKIMPL_H
