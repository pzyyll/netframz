//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_IMPL_H_
#define NETFRAMZ_NF_EVENT_TIMER_IMPL_H_

#include <functional>
#include <sys/time.h>

#include "nf_event_config.h"

class EventLoop;

class TimerTask;

class TimerImpl {
public:
    typedef unsigned long long id_type;
    typedef std::function<void(EventLoop *, task_data_t, int)> handle_t;
    typedef TimerTask *timer_task_ptr;

public:
    TimerImpl(const unsigned long interval, bool is_loop);

    ~TimerImpl();

    void Bind(handle_t handle);

    struct timeval GetExpireTime();

    void Process(EventLoop *loop, int mask);

    void set_id(id_type id) { id_ = id; }

    id_type get_id() { return id_; }

    void set_begin(struct timeval begin) { begin_ = begin; }

    void set_interval(unsigned long interval) { interval_ = interval; }

    void set_is_loop(bool is_loop) { is_loop_ = is_loop; }

    bool get_is_loop() { return is_loop_; }

    void set_data(task_data_t data) { data_ = data; }

    task_data_t get_data() { return data_; }

private:
    //default = 0
    id_type id_;
    struct timeval begin_;

    //ms, expire time = begin_ + interval_;
    unsigned long interval_;
    bool is_loop_;
    handle_t op_;
    task_data_t data_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_IMPL_H_

