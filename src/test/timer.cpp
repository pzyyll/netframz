#include <iostream>
#include <unistd.h>
#include "nf_event.h"
#include "nf_event_timer.h"

void cbfunc(EventLoop &loop, Timer &task, int mask) {
  std::cout << "Timer no class: " <<  mask << std::endl;
}

class Cb {
 public:
  void cbfunc(EventLoop &loop, Timer &task, int mask) {
    std::cout << "Timer: " <<  mask << i++ << std::endl;
    if (105 == i)
      task.Stop();
  }

  int i=0;
};

int main() {
  Cb cb;
  EventLoop loop;
  Timer timer1(loop, 1000, 1);
  timer1.Bind(&Cb::cbfunc, cb);
  timer1.Start();

  Timer timer2(loop, 100, 1);
  timer2.Bind(cbfunc);
  timer2.Start();

  Cb cb3;
  cb3.i = 100;
  Timer timer3(loop, 3000, 1);
  timer3.Bind(&Cb::cbfunc, cb3);
  timer3.Start();

  Cb cb4;
  cb4.i = 1000;
  Timer timer4(loop, 4000, 1);
  timer4.Bind(&Cb::cbfunc, cb4);
  timer4.Start();
  timer4.Stop();

  loop.Run();
  return 0;
}
