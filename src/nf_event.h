//
// Created by CaiZhili on 2017/6/29.
// Brief
//

#ifndef NETFRAMZ_NF_EVENT_H
#define NETFRAMZ_NF_EVENT_H

#include <unordered_map>
#include <queue>
#include <string>

#ifdef LINUX_EPOLL
#include "nf_epoll.h"
#endif

//#include "nf_event_task.h"
#include "nf_event_timer_mng.h"

class IOTaskImpl;

class EventLoop {
 public:
  typedef IOTaskImpl* iotask_pointer;
  typedef TimerImpl* timer_pointer;
  typedef std::unordered_map<int, iotask_pointer> TaskMap;
  typedef TaskMap::iterator TaskMapItr;

 public:
  EventLoop();
  ~EventLoop();

  void Run();

  const std::string& get_err_msg();

// protected:
  int SetIOTask(iotask_pointer task);
  int ResetIOTask(iotask_pointer task);
  int DelIOTask(int fd);

  int AddTimerTask(timer_pointer timer);
  int DelTimerTask(const unsigned long id);
  int ResetTimerTask(timer_pointer timer);

 private:
  void HandleIOEvent();
  bool HaveIOEvent();
  bool FindTask(const int fd, iotask_pointer *find);

  void HandleAllTimerTask();
  bool HaveTimerTask();

  void set_err_msg(std::string msg);

 private:
  bool stop_;

  TaskMap file_tasks_;

  poll_type poll_;

  std::deque<FiredEvent> fires;

  TimerMng timer_mng_;

  std::string err_msg_;
};

#endif //NETFRAMZ_NF_EVENT_H
