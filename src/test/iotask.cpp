#include <iostream>
#include <functional>
#include <unistd.h>
#include "nf_event.h"
#include "nf_event_iotask.h"

using namespace std::placeholders;

void cbfunc(EventLoop *loop, task_data_t data, int mask) {
  IOTask &task = *((IOTask *)data.data.ptr);

  std::cout << task.GetFd() << std::endl;
  std::cout << mask << std::endl;
  task.Stop();
}

class Cb {
 public:
    void CbFunc(EventLoop *loop, task_data_t data, int mask) {
      IOTask &task = *((IOTask *)data.data.ptr);
      std::cout << task.GetFd() << std::endl;

         std::cout << mask << std::endl;
         std::cout << "i=" << i << std::endl;
         task.Stop();
     }

     int i;
};

int main() {
  EventLoop loop;
  IOTask task(loop, STDIN_FILENO, EV_POLLIN);
  task_data_t data;
  data.data.ptr = &task;
  task.SetPrivateData(data);
  task.Bind(cbfunc);
  task.Start();

  Cb cb;
  cb.i = 33;
  IOTask taskout(loop, STDOUT_FILENO, EV_POLLOUT);
  data.data.ptr = &taskout;
  taskout.SetPrivateData(data);
  taskout.Bind(std::bind(&Cb::CbFunc, cb, _1, _2, _3));
  taskout.Start();
  loop.Run();
  return 0;
}
