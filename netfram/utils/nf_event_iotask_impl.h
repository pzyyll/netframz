//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_IOTASKIMPL_H
#define NETFRAMZ_NF_EVENT_IOTASKIMPL_H

#include <memory>
#include <functional>
#include "nf_event_config.h"

class EventService;

class IOTaskImpl;

class IOTaskImpl {
public:
    typedef std::function<void(EventService *, task_data_t data, int)> handle_t;

public:
    IOTaskImpl();

    IOTaskImpl(const int fd, const int mask,
               handle_t read_op_ = handle_t(),
               handle_t write_op_ = handle_t());

    ~IOTaskImpl();

    /*
     * @Brief prepare depression
     */
    void Bind(handle_t handle);

    void Process(EventService *es, int mask);

    void set_fd(int fd);

    int get_fd();

    void set_mask(int mask);

    int get_mask();

    void set_data(task_data_t data);

    task_data_t get_data();

    /*
     * @Brief Add todo
     */
    void AddMask(int mask);

    void RemoveMask(int mask);

    void SetWriteHandle(handle_t write_op);

    void SetReadHandle(handle_t read_op);

private:
    int fd_;
    int mask_;
    handle_t read_op_;
    handle_t write_op_;
    task_data_t data_;
};

#endif //NETFRAMZ_NF_EVENT_IOTASKIMPL_H
