//
// @Created by CaiZhili on 2017/7/12.
// @bref
//

#ifndef NETFRAMZ_NF_EVENT_TIMER_TASK_H
#define NETFRAMZ_NF_EVENT_TIMER_TASK_H

#include <functional>
#include "nf_event_config.h"

class TimerImpl;

class EventService;

class TimerTask {
    typedef TimerImpl impl_type;
public:
    typedef std::function<void(EventService *, task_data_t, int)> handle_t;
public:
    TimerTask(EventService &es, const unsigned long interval, bool is_es);

    ~TimerTask();

    // 绑定回调函数
    void Bind(handle_t handle);

    // 开始定时器
    int Start();

    // 重新开始
    int Restart();

    // 提前停止
    void Stop();

    void SetPrivateData(task_data_t data);

    task_data_t GetPrivateData();

private:
    EventService &es_;
    impl_type *impl_;
};

#endif //NETFRAMZ_NF_EVENT_TIMER_TASK_H
