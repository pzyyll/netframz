/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#ifndef NF_TEST_SERVER_SERVER_H
#define NF_TEST_SERVER_SERVER_H

#include "../src/nf_event.h"
#include "../src/nf_event_task.h"

class TServer {
 public:
  int Init(int argc, char **argv);

  int Run();

  void OnAccept(EventLoop &loopsv, IOTask &task, int mask);
  void OnRead(EventLoop &loopsv, IOTask &task, int mask);
  void OnRecv(EventLoop &loopsv, IOTask &task, int mask);

  void OnTick();
  void OnIdle();
  void OnExit();

 private:
  EventLoop loop;
}

#endif //NF_TEST_SERVER_SERVER_H
