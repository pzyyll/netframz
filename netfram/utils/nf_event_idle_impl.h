//
// @Create by CaiZhili on 20170804
// @Brief nf_event_idle_impl.h
//

#ifndef NF_SRC_IDLE_IMPL_H
#define NF_SRC_IDLE_IMPL_H

#include <functional>
#include "nf_event_config.h"

class EventService;

class IdleImpl {
public:
    typedef std::function<void (EventService *, task_data_t, int)> Handler;

    IdleImpl(Handler op);

    ~IdleImpl();

    void SetData(task_data_t data);

    task_data_t GetData();

    void SetOp(Handler op);

    Handler GetOp();

    void Process(EventService *es, int mask);

private:
    task_data_t data_;
    Handler op_;
};

#endif //NF_SRC_IDLE_TASK_IMPL_H
