/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#ifndef NF_TEST_SERVER_SERVER_H
#define NF_TEST_SERVER_SERVER_H

#include <unordered_map>
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
  typedef IOTask* IOTaskPtr;
  typedef TimerTask* TimerTaskPtr;
  typedef Connector* ConnectorPtr;
  typedef std::unordered_map<unsigned long, ConnectorPtr> conn_map_t;
  typedef std::unordered_map<unsigned long, TimerTaskPtr> timer_map_t;

 public:
  TServer();
  virtual ~TServer();

  int Init(int argc, char **argv);
  int StartListen();
  int StartTick();

  void OnAccept(EventLoop *loopsv, task_data_t data, int mask);
  //void OnRead(EventLoop *loopsv, task_data_t data, int mask);
  //void OnWriteRemain(EventLoop *loopsv, task_data_t data, int mask);
  void OnTick(EventLoop *loopsv, task_data_t data, int mask);
  void OnTimerOut(EventLoop *loopsv, task_data_t data, int mask);

  //int Run();
  //void OnTick();
  //void OnIdle();
  //void OnExit();

 private:
  int GetOption(int argc, char **argv);

  int MakeNonblock(int fd);
  int SetCliOpt(int fd);

 private:
  char *conf_file_;
  IOTaskPtr accept_task_;
  TimerPtr tick_;
  EventLoop loop_;
  iotask_map_t conn_map_;
  timer_map_t timer_map_;
};

#endif //NF_TEST_SERVER_SERVER_H
