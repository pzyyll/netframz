//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_iotask_impl.h"

IOTaskImpl::IOTaskImpl()
        : fd_(-1),
          mask_(0),
          read_op_(),
          write_op_(),
          data_() {

}

IOTaskImpl::IOTaskImpl(const int fd, const int mask,
                       handle_t read_op, handle_t write_op)
        : fd_(fd), mask_(mask), read_op_(read_op), write_op_(write_op) {

}

IOTaskImpl::~IOTaskImpl() {

}

void IOTaskImpl::Bind(handle_t handle) {
    read_op_ = handle;
}

void IOTaskImpl::Process(EventService *es, int mask) {
    if ((mask & EV_READABLE) && read_op_) {
        read_op_(es, data_, mask);
    }

    if ((mask & EV_WRITEABLE) && write_op_) {
        write_op_(es, data_, mask);
    }
    //TODO other 2017/10/23
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

void IOTaskImpl::AddMask(int mask) {
    mask_ |= mask;
}

void IOTaskImpl::RemoveMask(int mask) {
    mask_ &= ~mask;
}

void IOTaskImpl::SetWriteHandle(handle_t write_op) {
    write_op_ = write_op;
}

void IOTaskImpl::SetReadHandle(handle_t read_op) {
    read_op_ = read_op;
}
