//
// @Created by CaiZhili on 2017/6/30.
// @bref 
//

#include "nf_event_task.h"
#include "nf_event.h"

void IOTask::Start(EventLoop &loop, IOTaskPtr io_task) {
  loop.SetIOTask(io_task);
}

void IOTask::Stop(EventLoop &loop, IOTaskPtr io_task) {
  loop.DelIOTask(io_task->get_fd());
}

IOTaskPtr IOTask::make_task(int fd, int mask, Handle op) {
  return std::make_shared<IOTask>(fd, mask, op);
}

void IOTask::Process(EventLoop &loop, IOTaskPtr io_task, int mask) {
  op_(loop, io_task, mask);
}

template<typename Func, typename Obj>
void IOTask::Bind(Func &&func, Obj &&obj) {
  op_ = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void IOTask::Bind(IOTask::Handle handle) {
  op_ = handle;
}
