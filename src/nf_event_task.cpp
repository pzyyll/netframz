//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_task.h"
#include "nf_event.h"

IOTask::IOTask() : fd_(-1), mask_(0) {

}

IOTask::IOTask(int fd, int mask) : fd_(fd), mask_(mask) {

}

IOTask::IOTask(int fd, int mask, Handle op) : fd_(fd), mask_(mask), op_(op) {

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

void IOTask::Start(EventLoop &loop) {
  loop.SetIOTask(*this);
}

void IOTask::Stop(EventLoop &loop) {
  loop.DelIOTask(fd_);
}

void IOTask::Process(EventLoop &loop, IOTask &io_task, int mask) {
  if (op_)
    op_(loop, io_task, mask);
}
