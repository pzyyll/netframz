//
// @Create by CaiZhili on 20170804
// @Brief nf_event_idle_task.cpp
//
#include "nf_event_idle_task.h"
#include "nf_event.h"
#include "nf_event_idle_impl.h"

IdleTask::IdleTask(EventService &es, handle_t handle)
    : es_(es),
      pimpl_(new impl_type(handle)) {

}

IdleTask::~IdleTask() {
    if (pimpl_) {
        Stop();
        delete pimpl_;
    }
}

void IdleTask::Bind(handle_t handle) {
    pimpl_->SetOp(handle);
}

int IdleTask::Start() {
    return es_.AddIdleTask(pimpl_);
}

int IdleTask::Stop() {
    return es_.DelIdleTask(pimpl_);
}

void IdleTask::SetPrivateData(task_data_t data) {
    pimpl_->SetData(data);
}

task_data_t IdleTask::GetPrivateData() {
    return pimpl_->GetData();
}
