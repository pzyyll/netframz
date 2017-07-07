//
// Created by CaiZhili on 2017/6/29.
//

#include <type_traits>

#include "nf_event.h"

EventLoop::EventLoop() : stop_(false) {
  poll_.Init();
}

void EventLoop::Run() {
  do {

    HandleAllTimerTask();
   
    if (HaveIOEvent()) HandleIOEvent();

  } while (!stop_ && (HaveIOEvent()));
}

int EventLoop::SetIOTask(IOTask &task) {
  int ret = RET::RET_SUCCESS;

  IOTask find;
  if (FindTask(task.get_fd(), find)) {
    ret = poll_.ModEvent(task.get_fd(), task.get_fd(), task.get_mask());
    if (ret != RET::RET_SUCCESS) {
      set_err_msg(poll_.get_err());
      return ret;
    }
    file_tasks_[task.get_fd()] = task;
    return ret;
  }

  ret = poll_.AddEvent(task.get_fd(), task.get_fd(), task.get_mask());
  if (ret != RET::RET_SUCCESS) {
    set_err_msg(poll_.get_err());
    return ret;
  }
  file_tasks_[task.get_fd()] = task;

  return RET_SUCCESS;
}

int EventLoop::DelIOTask(int fd) {
  int ret = RET::RET_FAIL;

  IOTask find;
  if (FindTask(fd, find)) {
    ret = poll_.DelEvent(fd);
    if (ret != RET::RET_SUCCESS) {
      set_err_msg(poll_.get_err());
      return ret;
    }
    file_tasks_.erase(fd);
    return RET::RET_SUCCESS;
  }

  set_err_msg("Not Find Task!");
  return ret;
}

void EventLoop::HandleIOEvent() {
  int nds = poll_.WaitEvent(fires);

  //todo asyn or limit
  while (!fires.empty() && nds > 0) {
    FiredEvent &fire = fires.back();
    IOTask task;
    if (FindTask(fire.id, task)) {
      //Callback
      task.Process(*this, task, fire.mask);
    }
    fires.pop_back();
  }
}

bool EventLoop::HaveIOEvent() {
  return (!file_tasks_.empty() ? true : false);
}

bool EventLoop::FindTask(const int fd, IOTask &find) {
  TaskMapItr itr = file_tasks_.find(fd);
  if (itr != file_tasks_.end()) {
     find = itr->second;
     return true;
  }
  return false;
}

const std::string &EventLoop::get_err_msg() {
  return err_msg_;
}

void EventLoop::set_err_msg(std::string msg) {
  err_msg_ = msg;
}

int EventLoop::AddTimerTask(Timer &timer) {
  return timer_mng_.AddTimer(timer);
}

void EventLoop::HandleAllTimerTask() {
  std::vector<Timer> fire_timers;
  timer_mng_.GetFiredTimers(fire_timers);

  for (int i = 0; i < (int)fire_timers.size(); ++i) {
    Timer &timer = fire_timers[i];
    timer.Process(*this, timer, 0);
    if (timer.get_is_loop()) {
      struct timeval now;
      gettimeofday(&now, NULL);
      timer.set_begin(now);
      AddTimerTask(timer);
    }
  }
}


