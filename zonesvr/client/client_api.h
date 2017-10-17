//
// @Create by CaiZhili on 20170814
// @Brief
//
#ifndef NF_ZONESVR_TEST_CLIENT_API_H
#define NF_ZONESVR_TEST_CLIENT_API_H

#include <iostream>
#include <string>
#include <map>
#include <deque>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#include "proto.h"
#include "proto/zonesvr.pb.h"

#define ERR_MSG_MAX_LEN 256

struct Client {
    int  fd;
    char conn_ip[64];
    int  conn_port;
};

struct Pos {
    int x, y;
};

int Connect(Client *cli);
int Writen(int fd, const void *buff, const unsigned int lenth);
int Readn(int fd, void *buff, const unsigned int lenth);
int MakeBlockStatus(int fd, bool block);

int ProcessBuff(const char *buff, const unsigned int lenth);
int ProcessCmd(proto::Cmd &cmd);
int ProcessLoginRsp(const std::string &data);
int ProcessZoneSynRsp(const std::string &data);
int ProcessZoneSyn(const std::string &data);
int ProcessZoneUserRemove(const std::string &data);
int ProcessChatRsp(const std::string &data);
int ProcessChatStat(const std::string &data);
void FreshShow();

int Login();
void Logout();
int Move(Pos mv_direct);
int Chat();

int SendMsgToSvr(::google::protobuf::Message &msg, unsigned int type);

void CliRun(Client *cli);

#endif //NF_ZONESVR_TEST_CLIENT_API_H
