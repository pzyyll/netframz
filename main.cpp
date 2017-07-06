#include <iostream>
#include <unistd.h>
#include "src/nf_event.h"

void cbfunc(EventLoop &loop, IOTask &task, int mask) {
  std::cout << task->get_fd() << std::endl;
  std::cout << mask << std::endl;
  task.Stop(loop);
}

int main() {
  EventLoop loop;
  IOTask task(STDIN_FILENO, EV_POLLIN);
  task.Bind(cbfunc);
  task.Start(loop);
  loop.Run();
  return 0;
}