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

typedef void (*FileRwCb)(EventLoop &loop, Task &io_task, int mask);

class IOTask {
  friend class EventLoop;
  typedef Handle FileRwCb;

 public:
  IOTask(int fd, int mask, Handle op) : fd_(fd), mask_(mask),op_(op) { }

  void set_fd(int fd) { fd_ = fd; }
  void set_mask(int mask) { mask_ = mask; }
  void set_cb(Handle cb) { op_ = cb; }

  int get_fd() { return fd_; }
  int get_mask() { return mask_; }
  Handle get_cb() { return op_; }

  static void Start(EventLoop &loop, IOTask &io_task);
  static void Stop(EventLoop &loop, IOTask &io_task);

 protected:
  void Process(EventLoop &loop, IOTask &io_task, int mask);

 private:
  int fd_;
  int mask_;
  Handle op_;
};

typedef std::shared_ptr<IOTask> TaskPtr;

#endif //NETFRAMZ_NF_EVENT_TASK_H
