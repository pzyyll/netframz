#include <iostream>
#include <unistd.h>
#include "src/nf_event.h"

void cbfunc(EventLoop &loop, IOTask &task, int mask) {
  std::cout << task.get_fd() << std::endl;
  std::cout << mask << std::endl;
  task.Stop(loop);
}

class Cb {
 public:
     void CbFunc(EventLoop &loop, IOTask &task, int mask) {
         std::cout << task.get_fd() << std::endl;
         std::cout << mask << std::endl;
         std::cout << "i=" << i << std::endl;
         task.Stop(loop);
     }

     int i;
};

int main() {
  EventLoop loop;
  IOTask task(STDIN_FILENO, EV_POLLIN);
  task.Bind(cbfunc);
  task.Start(loop);

  Cb cb;
  cb.i = 33;
  IOTask taskout(STDOUT_FILENO, EV_POLLOUT);
  taskout.Bind(&Cb::CbFunc, cb);
  taskout.Start(loop);
  loop.Run();
  return 0;
}
