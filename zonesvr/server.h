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
#include <sys/resource.h>

#include "nf_event.h"
#include "nf_event_iotask.h"
#include "nf_event_timer_task.h"
#include "nf_thread.h"
#include "nf_mutex.h"
#include "connector.h"
#include "acceptor.h"
#include "proto.h"
#include "timer_task_mgr.h"

class Server {
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
    Server();

    virtual ~Server();

    virtual int Init(int argc, char **argv);

    void Run();

    void Stop();

protected:
    int StartListen();

    int StartTick();

    void OnAccept(int fd, ErrCode &err);

    void Do(int fd);

    void OnRead(unsigned long lenth, task_data_t data, ErrCode err);

    void OnWriteErr(unsigned long lenth, task_data_t data, ErrCode err);

    void OnTick(EventService *es, task_data_t data, int mask);

    void OnTimerOut(EventService *es, task_data_t data, int mask);

    int GetOption(int argc, char **argv);

    int MakeNonblock(int fd);

    int SetCliOpt(int fd);

    int SetMaxFds(int max_fds);

    ConnectorPtr CreateConn(int fd);

    void DelConn(unsigned long cid);

    void DelConn(ConnectorPtr conn);

    ConnectorPtr FindConn(unsigned long cid);

    int CheckMask(int mask);

    bool CheckConnect(unsigned long cid);

    int Daemon();

    int Response(proto::Cmd &cmd, unsigned long cid);

    int Response(const char *buff, unsigned long size, unsigned long cid);

    void Parse(Connector &conn);

    virtual void ProcessCmd(proto::Cmd &cmd, const unsigned long cid);

    virtual void Tick(unsigned long now);

    virtual void CloseConn(unsigned long cid);

private:
    char         *conf_file_;
    TimerTaskPtr tick_;
    EventService es_;
    conn_map_t   conn_map_;
    TimerTaskMgr timer_mgr_;

    Acceptor     acceptor_;
};

#endif //NF_TEST_SERVER_SERVER_H
