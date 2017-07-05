//
// @Created by CaiZhili on 2017/6/30.
// @bref 
//

#ifndef NETFRAMZ_NF_EVENT_TASK_H
#define NETFRAMZ_NF_EVENT_TASK_H

#include <memory>

#include "nf_event_config.h"

struct FiredTask {
  int id;
  int mask;
};

class EventLoop;

class IOTask;

typedef std::shared_ptr<IOTask> TaskPtr;

typedef void (*FileRwCb)(EventLoop &loop, TaskPtr io_task, int mask);

class IOTask {
  friend class EventLoop;

 public:
  typedef FileRwCb Handle;

 public:
  IOTask(int fd, int mask, Handle op) : fd_(fd), mask_(mask),op_(op) { }

  void set_fd(int fd) { fd_ = fd; }
  void set_mask(int mask) { mask_ = mask; }
  void set_cb(Handle cb) { op_ = cb; }

  int get_fd() { return fd_; }
  int get_mask() { return mask_; }
  Handle get_cb() { return op_; }

  //todo
  static void Start(EventLoop &loop, TaskPtr io_task);
  static void Stop(EventLoop &loop, TaskPtr io_task);
  static TaskPtr make_task(int fd, int mask, Handle op);

 protected:
  void Process(EventLoop &loop, TaskPtr io_task, int mask);

 private:
  int fd_;
  int mask_;
  Handle op_;
};

#endif //NETFRAMZ_NF_EVENT_TASK_H
