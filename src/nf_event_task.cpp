//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_task.h"
#include "nf_event.h"

IOTask::IOTask(EventLoop &loop)
    : loop_(loop), fd_(-1), mask_(0) {

}

IOTask::IOTask(EventLoop &loop, const int fd, const int mask)
    : loop_(loop), fd_(fd), mask_(mask) {

}

IOTask::IOTask(EventLoop &loop, const int fd, const int mask, Handle op)
    : loop_(loop), fd_(fd), mask_(mask), op_(op) {

}

IOTask::~IOTask() {
  loop_.DelIOTask(fd_);
}

void IOTask::Bind(IOTask::Handle handle) {
  op_ = handle;
}

void IOTask::set_fd(int fd) {
  fd_ = fd;
}

int IOTask::get_fd() {
  return fd_;
}

void IOTask::set_mask(int mask) {
  mask_ = mask;
}

int IOTask::get_mask() {
  return mask_;
}

void IOTask::set_data(task_data_t data) {
  data_ = data;
}

task_data_t IOTask::get_data() {
  return data_;
}

void IOTask::Start() {
  loop_.SetIOTask(this);
}

void IOTask::Restart() {
  loop_.ResetIOTask(this);
}

void IOTask::Stop() {
  loop_.DelIOTask(fd_);
}

void IOTask::Process(EventLoop &loop, IOTask &io_task, int mask) {
  if (op_)
    op_(loop, io_task, mask);
}
