//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_iotask.h"
#include "nf_event.h"
#include "nf_event_iotask_impl.h"

#include "mem_check.h"

IOTask::IOTask(EventService &es, const int fd)
        : es_(es), pimpl_(new IOTaskImpl(fd, 0)) {

}

IOTask::IOTask(EventService &es, const int fd, const int mask,
               handle_t read_op,
               handle_t write_op)
        : es_(es), pimpl_(new IOTaskImpl(fd, mask, read_op, write_op)) {

}

IOTask::~IOTask() {
    Stop();
    if (pimpl_) {
        delete pimpl_;
        pimpl_ = NULL;
    }
}

void IOTask::Bind(handle_t handle) {
    pimpl_->Bind(handle);
}

int IOTask::Start() {
    return es_.SetIOTask(pimpl_);
}

int IOTask::Restart() {
    return es_.ResetIOTask(pimpl_);
}

void IOTask::Stop() {
    es_.DelIOTask(pimpl_->get_fd());
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

std::string IOTask::GetErr() {
    return es_.get_err_msg();
}

/*
 * @Brief New
 */
void IOTask::AddMask(int mask) {
    pimpl_->AddMask(mask);
}

void IOTask::RemoveMask(int mask) {
    pimpl_->RemoveMask(mask);
}

void IOTask::SetReadHandle(handle_t read_op) {
    pimpl_->SetReadHandle(read_op);
}

void IOTask::SetWriteHandle(handle_t write_op) {
    pimpl_->SetWriteHandle(write_op);
}
