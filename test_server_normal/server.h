/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#ifndef NF_TEST_SERVER_SERVER_H
#define NF_TEST_SERVER_SERVER_H

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "nf_event.h"
#include "nf_event_iotask.h"
#include "nf_event_timer_task.h"
#include "connector.h"
#include "sigleton.h"

class TServer {
 public:
  enum STAT {
    FAIL = -1,
    SUCCESS = 0,
  };

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
  int GetOption(int argc, char **argv);

 private:
  char *conf_file_;
  IOTask *accept_task_;
  EventLoop loop_;
};

#endif //NF_TEST_SERVER_SERVER_H
