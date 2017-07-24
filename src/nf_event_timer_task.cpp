//
// @Created by CaiZhili on 2017/7/12.
// @bref
//

#include <sys/time.h>

#include "nf_event_timer_task.h"
#include "nf_event.h"
#include "nf_event_timer_impl.h"

TimerTask::TimerTask(EventLoop &loop, const unsigned long interval, bool is_loop)
        : loop_(loop), impl_(new impl_type(interval, is_loop)) {

}

TimerTask::~TimerTask() {
    if (impl_)
        delete impl_;
    impl_ = NULL;
}

void TimerTask::Bind(handle_t handle) {
    impl_->Bind(handle);
}

void TimerTask::Start() {
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    impl_->set_begin(now);

    loop_.AddTimerTask(impl_);
}

void TimerTask::Restart() {
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    impl_->set_begin(now);

    loop_.ResetTimerTask(impl_);
}

void TimerTask::Stop() {
    loop_.DelTimerTask(impl_->get_id());
}

void TimerTask::SetPrivateData(task_data_t data) {
    impl_->set_data(data);
}

task_data_t TimerTask::GetPrivateData() {
    return impl_->get_data();
}
