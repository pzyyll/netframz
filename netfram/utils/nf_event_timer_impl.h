//
// @Created by CaiZhili on 2017/7/6.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_IMPL_H_
#define NETFRAMZ_NF_EVENT_TIMER_IMPL_H_

#include <functional>
#include <sys/time.h>

#include "nf_event_config.h"

class EventService;

class TimerTask;

class TimerImpl {
public:
    typedef unsigned long long id_type;
    typedef std::function<void(EventService *, task_data_t, int)> handle_t;
    typedef TimerTask *timer_task_ptr;

public:
    TimerImpl(const unsigned long interval, bool is_es);

    ~TimerImpl();

    void Bind(handle_t handle);

    struct timeval GetExpireTime();

    void Process(EventService *es, int mask);

    void set_id(id_type id) { id_ = id; }

    id_type get_id() { return id_; }

    void set_begin(struct timeval begin) { begin_ = begin; }

    void set_interval(unsigned long interval) { interval_ = interval; }

    void set_is_es(bool is_es) { is_es_ = is_es; }

    bool get_is_es() { return is_es_; }

    void set_data(task_data_t data) { data_ = data; }

    task_data_t get_data() { return data_; }

private:
    //default = 0
    id_type        id_;
    struct timeval begin_;

    //ms, expire time = begin_ + interval_;
    unsigned long  interval_;
    bool           is_es_;
    handle_t       op_;
    task_data_t    data_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_IMPL_H_

