//
// Created by CaiZhili on 2017/6/29.
//

#include <type_traits>

#include "nf_event.h"

EventLoop::EventLoop() : stop_(false) {
  poll_.Init();
}

void EventLoop::Run() {
  while (!stop_ && !file_tasks_.empty()) {
    std::deque<FiredTask> fires;
    int nds = poll_.WaitEvent(fires);
    while (!fires.empty() && nds > 0) {
      FiredTask &fire = fires.back();
      TaskPtr find = FindTask(fire.id);
      if (find) {
        find->Process(*this, find, fire.mask);
      }
      fires.pop_back();
    }
  }
}

int EventLoop::SetIOTask(TaskPtr task) {
  int ret = RET::RET_SUCCESS;

  TaskPtr find = FindTask(task->get_fd());
  if (find) {
    ret = poll_.ModEvent(task->get_fd(), task->get_fd(), task->get_mask());
    if (ret != RET::RET_SUCCESS) {
      set_err_msg(poll_.get_err());
      return ret;
    }
    file_tasks_[task->get_fd()] = task;
    return ret;
  }

  ret = poll_.AddEvent(task->get_fd(), task->get_fd(), task->get_mask());
  if (ret != RET::RET_SUCCESS) {
    set_err_msg(poll_.get_err());
    return ret;
  }
  file_tasks_[task->get_fd()] = task;

  return RET_SUCCESS;
}

int EventLoop::DelIOTask(int fd) {
  int ret = RET::RET_FAIL;

  TaskPtr find = FindTask(fd);
  if (find) {
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

TaskPtr EventLoop::FindTask(int fd) {
  TaskMap::iterator itr = file_tasks_.find(fd);
  if (itr != file_tasks_.end())
    return itr->second;
  return TaskPtr();
}
const std::string &EventLoop::get_err_msg() {
  return err_msg_;
}

void EventLoop::set_err_msg(std::string msg) {
  err_msg_ = msg;
}
