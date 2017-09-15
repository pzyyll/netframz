#include <iostream>
#include <functional>
#include <unistd.h>
#include "nf_event.h"
#include "nf_event_timer_task.h"

using namespace std::placeholders;

struct PriData {
    int fd;
    unsigned long expired;
} data_test;

void cbfunc(EventService *es, task_data_t data, int mask) {
  std::cout << "Timer no class: " <<  mask << std::endl;
}

class Cb {
 public:
  void cbfunc(EventService *es, task_data_t data, int mask) {

    PriData *d = (PriData *)data.data.ptr;
    std::cout << "pri: " << d->fd << std::endl;
  }

  int i=0;
};

int main() {
  Cb cb;
  EventService es;
  TimerTask timer1(es, 1000, 1);
  timer1.Bind(std::bind(&Cb::cbfunc, cb, _1, _2, _3));
  //timer1.Start();

  TimerTask timer2(es, 100, 1);
  timer2.Bind(cbfunc);
  //timer2.Start();

  Cb cb3;
  cb3.i = 100;
  TimerTask timer3(es, 3000, 1);
  timer3.Bind(std::bind(&Cb::cbfunc, cb3, _1, _2, _3));
  //timer3.Start();

  Cb cb4;
  cb4.i = 1000;
  TimerTask timer4(es, 4000, 0);
  timer4.Bind(std::bind(&Cb::cbfunc, cb4, _1, _2, _3));
  task_data_t data4;
  data_test.fd = 13;
  data4.data.ptr = &data_test;
  timer4.SetPrivateData(data4);
  timer4.Start();

  es.Run();
  return 0;
}
