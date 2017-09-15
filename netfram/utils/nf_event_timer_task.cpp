//
// @Created by CaiZhili on 2017/7/12.
// @bref
//

#include <sys/time.h>

#include "nf_event_timer_task.h"
#include "nf_event.h"
#include "nf_event_timer_impl.h"

#include "mem_check.h"

TimerTask::TimerTask(EventService &es, const unsigned long interval, bool is_es)
        : es_(es), impl_(new impl_type(interval, is_es)) {

}

TimerTask::~TimerTask() {
    if (impl_) {
        Stop();
        delete impl_;
    }
    impl_ = NULL;
}

void TimerTask::Bind(handle_t handle) {
    impl_->Bind(handle);
}

int TimerTask::Start() {
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    impl_->set_begin(now);

    return es_.AddTimerTask(impl_);
}

int TimerTask::Restart() {
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    impl_->set_begin(now);

    return es_.ResetTimerTask(impl_);
}

void TimerTask::Stop() {
    es_.DelTimerTask(impl_->get_id());
}

void TimerTask::SetPrivateData(task_data_t data) {
    impl_->set_data(data);
}

task_data_t TimerTask::GetPrivateData() {
    return impl_->get_data();
}
