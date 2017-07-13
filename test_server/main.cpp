#include <iostream>
#include <functional>
#include <unistd.h>

#include "src/nf_event.h"

void cbfunc(EventLoop &loop, Timer &task, int mask) {
  std::cout << "Timer no class: " <<  mask << std::endl;
}

class Cb {
 public:
  void cbfunc(EventLoop &loop, Timer &task, int mask) {
    std::cout << "Timer: " <<  mask << i++ << std::endl;
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

  loop.Run();
  return 0;
}
