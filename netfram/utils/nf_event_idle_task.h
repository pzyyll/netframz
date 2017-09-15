//
// @Create by CaiZhili on 20170804
// @Brief nf_event_idle_task.h
//
#include <functional>
#include <string>
#include "nf_event_config.h"

class EventService;

class IdleImpl;

class IdleTask {
public:
    typedef std::function<void(EventService *, task_data_t, int)> handle_t;
    typedef IdleImpl impl_type;

    IdleTask(EventService &es, handle_t handle);

    ~IdleTask();

    void Bind(handle_t handle);

    int Start();

    int Stop();

    void SetPrivateData(task_data_t data);

    task_data_t GetPrivateData();

private:
    EventService &es_;
    impl_type *pimpl_;
};
