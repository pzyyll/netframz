//
// @Create by CaiZhili on 20170804
// @Brief nf_event_idle_task.cpp
//
#include "nf_event_idle_task.h"
#include "nf_event.h"
#include "nf_event_idle_impl.h"

IdleTask::IdleTask(EventLoop &loop, handle_t handle)
    : loop_(loop),
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
    return loop_.AddIdleTask(pimpl_);
}

int IdleTask::Stop() {
    return loop_.DelIdleTask(pimpl_);
}

void IdleTask::SetPrivateData(task_data_t data) {
    pimpl_->SetData(data);
}

task_data_t IdleTask::GetPrivateData() {
    return pimpl_->GetData();
}
