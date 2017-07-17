/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#ifndef NF_TEST_SERVER_SERVER_H
#define NF_TEST_SERVER_SERVER_H

#include "../src/nf_event.h"
#include "../src/nf_event_iotask.h"
#include "../src/nf_event_timer_task.h"

class TServer {
 public:
  TServer();
  virtual ~TServer();

  int Init(int argc, char **argv);

  void OnAccept(EventLoop *loopsv, task_data_t data, int mask);
  void OnRead(EventLoop *loopsv, task_data_t data, int mask);
  void OnRecv(EventLoop *loopsv, task_data_t data, int mask);
  void OnTick(EventLoop *loopsv, task_data_t data, int mask);

  int Run();
  void OnTick();
  //void OnIdle();
  void OnExit();

 private:

 private:
  EventLoop loop;
};

#endif //NF_TEST_SERVER_SERVER_H
