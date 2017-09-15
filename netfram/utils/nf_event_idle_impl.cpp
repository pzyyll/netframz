//
// @Create by Caizhili on 20170804
// @Brief nf_event_idle_impl.cpp
//

#include "nf_event_idle_impl.h"

IdleImpl::IdleImpl(Handler op) : op_(op) {

}

IdleImpl::~IdleImpl() {

}

void IdleImpl::SetData(task_data_t data) {
    data_ = data;
}

task_data_t IdleImpl::GetData() {
    return data_;
}

void IdleImpl::SetOp(Handler op) {
    op_ = op;
}

IdleImpl::Handler IdleImpl::GetOp() {
    return op_;
}

void IdleImpl::Process(EventService *es, int mask) {
    if (op_)
        op_(es, data_, mask);
}
