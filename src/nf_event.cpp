//
// Created by CaiZhili on 2017/6/29.
//

#include <type_traits>

#include "nf_event.h"

void EventLoop::Run() {
  while (!stop_) {
    std::deque<FiredTask> fires;
    int nds = poll_.WaitEvent(fires);
    while (!fires.empty()) {
      FiredTask &fire = fires.back();
      TaskMap::iterator iter = file_tasks_.find(fire.id);
      if (iter != file_tasks_.end()) {
        iter->second->Process(*this, *iter->second, fire.mask);
      }
      fires.pop_back();
    }
  }
}

int EventLoop::SetIOTask(IOTask &task) {

  return 0;
}
int EventLoop::DelIOTask(IOTask &task) {
  return 0;
}

template<typename T>
std::shared_ptr<T> EventLoop::FindTask(T &task) {
  std::shared_ptr<T> ptr;
  if (typeid(task) == typeid(IOTask)) {
    TaskMap::iterator itr = file_tasks_.find(task.get_fd());
    if (itr != file_tasks_.end())
      return itr->second;
  }
  return ptr;
}
