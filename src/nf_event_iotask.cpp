//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_iotask.h"
#include "nf_event.h"
#include "nf_event_iotask_impl.h"

IOTask::IOTask(EventLoop &loop)
    : loop_(loop), pimpl_(new IOTaskImpl(-1, 0)) {

}

IOTask::IOTask(EventLoop &loop, const int fd, const int mask)
    : loop_(loop), pimpl_(new IOTaskImpl(fd, mask)) {

}

IOTask::IOTask(EventLoop &loop, const int fd, const int mask, handle_t op)
    : loop_(loop), pimpl_(new IOTaskImpl(fd, mask, op)) {

}

IOTask::~IOTask() {
  Stop();
}

void IOTask::Bind(handle_t handle) {
  pimpl_->Bind(handle);
}

void IOTask::Start() {
  loop_.SetIOTask(pimpl_);
}

void IOTask::Restart() {
  loop_.ResetIOTask(pimpl_);
}

void IOTask::Stop() {
  loop_.DelIOTask(pimpl_->get_fd());
}

void IOTask::SetPrivateData(task_data_t data) {
  pimpl_->set_data(data);
}

task_data_t IOTask::GetPrivateData() {
  return pimpl_->get_data();
}

void IOTask::SetMask(int mask) {
  pimpl_->set_mask(mask);
}

int IOTask::GetMask() {
  return pimpl_->get_mask();
}

int IOTask::GetFd() {
  return pimpl_->get_fd();
}
