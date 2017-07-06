//
// @Created by CaiZhili on 2017/6/30.
// @bref
//

#include "nf_event_task.h"
#include "nf_event.h"

void IOTask::Start(EventLoop &loop) {
  loop.SetIOTask(*this);
}

void IOTask::Stop(EventLoop &loop) {
  loop.DelIOTask(fd_);
}

void IOTask::Bind(IOTask::Handle handle) {
  op_ = handle;
}

void IOTask::Process(EventLoop &loop, IOTask &io_task, int mask) {
  if (op_)
    op_(loop, io_task, mask);
}
