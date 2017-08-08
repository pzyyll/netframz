/*
 * @Created by Caizhili on 2017/7/13
 * @Bref
 */

#ifndef NF_TEST_SERVER_SERVER_H
#define NF_TEST_SERVER_SERVER_H

#include <unordered_map>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>

#include "nf_event.h"
#include "nf_event_iotask.h"
#include "nf_event_timer_task.h"
#include "connector.h"

class TServer {
public:
    enum Ret {
        FAIL    = -1,
        SUCCESS = 0,
    };
    typedef IOTask    *IOTaskPtr;
    typedef TimerTask *TimerTaskPtr;
    typedef Connector *ConnectorPtr;
    typedef std::unordered_map<unsigned long, ConnectorPtr> conn_map_t;
    typedef std::unordered_map<unsigned long, TimerTaskPtr> timer_map_t;

public:
    TServer();

    virtual ~TServer();

    int Init(int argc, char **argv);

    void Run();

    void Stop();

    void Do(Connector &conn);

    void Tick(unsigned long now);

    long int Response(unsigned long cid, const char *buff, unsigned long size);

    //void OnIdle();

    //void OnExit();

protected:
    int StartListen();

    int StartTick();

    void OnAccept(EventLoop *loopsv, task_data_t data, int mask);

    void OnRead(unsigned long lenth, task_data_t data, ErrCode err);

    void OnWriteErr(unsigned long lenth, task_data_t data, ErrCode err);

    void OnTick(EventLoop *loopsv, task_data_t data, int mask);

    void OnTimerOut(EventLoop *loopsv, task_data_t data, int mask);

    int GetOption(int argc, char **argv);

    int MakeNonblock(int fd);

    int SetCliOpt(int fd);

    ConnectorPtr CreateConn(int fd);

    void DelConn(unsigned long cid);

    void DelConn(ConnectorPtr conn);

    ConnectorPtr FindConn(unsigned long cid);

    int CheckMask(int mask);

    TimerTaskPtr CreateTimerTask(unsigned long cid);

    void DelTimerTask(unsigned long cid);

    TimerTaskPtr FindTimer(unsigned long cid);

    void CloseConn(unsigned long cid);

    int Daemon();

private:
    char         *conf_file_;
    IOTaskPtr    accept_task_;
    TimerTaskPtr tick_;
    EventLoop    loop_;
    conn_map_t   conn_map_;
    timer_map_t  timer_map_;
};

#endif //NF_TEST_SERVER_SERVER_H
