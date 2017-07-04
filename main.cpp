#include <iostream>
#include <unistd.h>
#include "src/nf_event.h"

void cbfunc(EventLoop &loop, IOTask &task, int mask) {
  std::cout << task.get_fd() << std::endl;
  std::cout << mask << std::endl;
  IOTask::Stop(loop, task);
}

int main() {
  EventLoop loop;
  TaskPtr ptr = IOTask::make_task(STDIN_FILENO, EV_POLLIN, cbfunc);
  IOTask::Start(loop, *ptr);
  loop.Run();
  return 0;
}