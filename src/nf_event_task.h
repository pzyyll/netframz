//
// @Created by CaiZhili on 2017/6/30.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TASK_H
#define NETFRAMZ_NF_EVENT_TASK_H

#include <memory>
#include <functional>
#include "nf_event_config.h"

struct FiredTask {
  int id;
  int mask;
};

class EventLoop;

class IOTask;

typedef std::shared_ptr<IOTask> IOTaskPtr;

//typedef void (*FileRwCb)(EventLoop &loop, IOTaskPtr io_task, int mask);
typedef std::function<void (EventLoop&, IOTask&, int)> FileRwCb;

class IOTask {
  friend class EventLoop;

 public:
  typedef FileRwCb Handle;

 public:
  IOTask(int fd, int mask) : fd_(fd), mask_(mask) { }
  IOTask(int fd, int mask, Handle op) : fd_(fd), mask_(mask), op_(op) { }

  template <typename Func, typename Obj>
  void Bind(Func &&func, Obj &&obj);
  void Bind(Handle handle);

  void set_fd(int fd) { fd_ = fd; }
  void set_mask(int mask) { mask_ = mask; }

  int get_fd() { return fd_; }
  int get_mask() { return mask_; }
  Handle get_cb() { return op_; }

  //todo
  void Start(EventLoop &loop);
  void Stop(EventLoop &loop);

 protected:
  void Process(EventLoop &loop, IOTask &io_task, int mask);

 private:
  int fd_;
  int mask_;
  Handle op_;
};

template<typename Func, typename Obj>
void IOTask::Bind(Func &&func, Obj &&obj) {
  op_ = std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

#endif //NETFRAMZ_NF_EVENT_TASK_H
