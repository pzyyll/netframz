#include <iostream>
#include <unistd.h>
#include "src/nf_event.h"

void cbfunc(EventLoop &loop, Timer &task, int mask) {
  std::cout << "Timer: " <<  mask << std::endl;
}

class Cb {
 public:
  void cbfunc(EventLoop &loop, Timer &task, int mask) {
    std::cout << "Timer: " <<  mask << i << std::endl;
  }

  int i;
};

int main() {
  EventLoop loop;
  Timer timer1(1000, 0);
  timer1.Bind(cbfunc);
  timer1.Start(loop);

  loop.Run();
  return 0;
}
