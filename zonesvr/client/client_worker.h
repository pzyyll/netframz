#ifndef NF_ZONESVR_CLIENT_CLIENT_WORKER_H
#define NF_ZONESVR_CLIENT_CLIENT_WORKER_H

#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <functional>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#include "nf_thread.h"
#include "nf_event.h"
#include "nf_event_iotask.h"
#include "socket.h"
#include "proto.h"
#include "proto/zonesvr.pb.h"

#define MAX_COL 20
#define MAX_ROW 20

struct Pos {
    int x, y;
};

class ClientWorker {
    const static int kBuffMaxLen = 1024 * 1024;
public:
    ClientWorker();

    ~ClientWorker();

    void Start(const Endpoint &ep, const std::string &name);

    void Wait();

    int SendMsgToSvr(::google::protobuf::Message &msg, unsigned int type);

    int CheckMask(int mask);
    void RcvCb(EventService *es, task_data_t data, int mask);
    void RcvHandler(void *args);

    void SndHandler(void *args);

    int ParseBuff(const char *buff, const unsigned int lenth);
    int ProcessCmd(proto::Cmd &cmd);
    int ProcessLoginRsp(const std::string &data);
    int ProcessZoneSynRsp(const std::string &data);
    int ProcessZoneSyn(const std::string &data);
    int ProcessZoneUserRemove(const std::string &data);
    int ProcessChatRsp(const std::string &data);
    int ProcessChatStat(const std::string &data);

    int Login(const std::string &name);
    void Logout();
    int Move(Pos mv_direct);
    int Chat();

    void RandOption(char &option);
    int InputOption();

private:
    nf::Thread snd_;
    nf::Thread rcv_;

    Socket socket_;
    Endpoint ep_;
    char rv_buff_[kBuffMaxLen];
    unsigned int rv_len_;
    int login_wait_;

    Persion self_info_;

    EventService es_;
    IOTask *rcv_task_;

    std::string name_;

    bool is_close_;
};

#endif //NF_ZONESVR_CLIENT_CLIENT_WORKER_H
