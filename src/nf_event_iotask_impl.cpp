//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_iotask_impl.h"

IOTaskImpl::IOTaskImpl()
    : fd_(-1), mask_(0) {

}

IOTaskImpl::IOTaskImpl(const int fd, const int mask)
    : fd_(fd), mask_(mask) {

}

IOTaskImpl::IOTaskImpl(const int fd, const int mask, handle_t op)
    : fd_(fd), mask_(mask), op_(op) {

}

IOTaskImpl::~IOTaskImpl() {

}

void IOTaskImpl::Bind(handle_t handle) {
  op_ = handle;
}

void IOTaskImpl::Process(EventLoop *loop, int mask) {
  if (op_)
    op_(loop, data_, mask);
}

void IOTaskImpl::set_fd(int fd) {
  fd_ = fd;
}

int IOTaskImpl::get_fd() {
  return fd_;
}

void IOTaskImpl::set_mask(int mask) {
  mask_ = mask;
}

int IOTaskImpl::get_mask() {
  return mask_;
}

void IOTaskImpl::set_data(task_data_t data) {
  data_ = data;
}

task_data_t IOTaskImpl::get_data() {
  return data_;
}

