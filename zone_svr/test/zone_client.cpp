#include <iostream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

#include "tcp_client.h"
#include "proto.h"
#include "proto/zonesvr.pb.h"

using namespace std;
using namespace proto;

char recvbuff[1024 * 1024 * 10];
unsigned int rv_len;
Persion *persion;
map<std::string, Persion *> persions_map;

int BuffHandle(const char *buff, const unsigned int len);
int SendMsgToSvr(TcpClient &cli, const ::google::protobuf::Message &msg, const unsigned int type);
int RecvFromSvr(TcpClient &cli, char *buff, unsigned int &len);

void ProcessLogin(TcpClient &cli);
void ProcessMove(TcpClient &cli);
//void ProcessChat(TcpClient &cli);

void ProcessRspCmd(Cmd &cmd);
void ProcessLoginRsp(const std::string &data);
void ProcessZoneSynRsp(const std::string &data);
void ProcessChatRsp(const std::string &data);
void ProcessZoneSyn(const std::string &data);
//void ProcessChatStat(const std::string &data);

void InputOption(const std::string &option);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);
    TcpClient cli;

    cli.init(argv[1], port, -1, -1);

    string a;
    while (cin >> a) {
    }

    return 0;
}

int BuffHandle(const char *buff, const unsigned int len) {
    unsigned int usepos = 0;
    while (usepos < len) {
        Cmd cmd;
        int nr = cmd.Parse(buff + usepos, len - usepos);
        if (nr < 0) {
            cout << cmd.GetErr() << endl;
            return -1;
        }

        if (nr == 0) {
            cout << cmd.GetErr() << endl;
            break;
        }

        cout << cmd.GetMsgData() << endl;
        ProcessRspCmd(cmd);

        usepos += nr;
    }

    return usepos;
}

int SendMsgToSvr(TcpClient &cli, const ::google::protobuf::Message &msg, const unsigned int type) {
    std::string str;
    req.SerializeToString(&str);

    Cmd cmd;
    cmd.SetType(type);
    cmd.SetMsgData(str);

    str.clear();
    cmd.Serialize(str);
    cout << "snd: " << str.size() << endl;
    int ns = cli.send((void *)str.c_str(), str.size());
    if (ns < 0) {
        cout << cli.get_err_msg() << endl;
        return -1;
    }
    return 0;
}

int RecvFromSvr(TcpClient &cli, char *recvbuff, unsigned int &rv_len) {
    unsigned int len = sizeof(recvbuff) - rv_len;
    int ret = cli.recv((void *)(recvbuff + rv_len), len);
    if (ret < 0) {
        cout << cli.get_err_msg() << endl;
        return -1;
    }
    cout << "recv: " <<  len << endl;
    rv_len += len;
    cout << "rv_len: " << rv_len << endl;
    return (int)len;
}

void ProcessRspCmd(Cmd &cmd) {
    switch (cmd.GetType()) {
        case MsgCmd::LOGIN_RSP:
            break;
        case MsgCmd::ZONE_SYN_RSP:
            break;
        case MsgCmd::ZONE_SYN:
            break;
        case MsgCmd::CHAT_RSP:
            break;
        case MsgCmd::CHAT_STAT:
            break;
        default:
            break;
    }
}

void ProcessLogin(const std::string &name) {
    LoginReq req;
    req.set_name(name);

    
}

void InputOption(const std::string &option) {
    //todo
    if (option == "h") {

    } else if (option == "l") {

    } else if (option == "j") {

    } else if (option == "k") {
        
    } else if (option == "u") {

    } else if (option == "q") {

    }
}
