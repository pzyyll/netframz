#include <iostream>
#include <map>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcp_client.h"
#include "proto.h"
#include "proto/zonesvr.pb.h"

#define MAX_X 100
#define MAX_Y 100

using namespace std;
using namespace proto;

TcpClient cli;
char recvbuff[1024 * 1024 * 10];
unsigned int rv_len;

int SendMsgToSvr(TcpClient &cli, const ::google::protobuf::Message &msg, const unsigned int type) {
    std::string str;
    msg.SerializeToString(&str);

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

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);

    if (cli.init(argv[1], port, 5000, 5000) < 0) {
        cout << "Tcp client init fail." << endl;
        return 0;
    }

    //LoginReq
    LoginReq req;
    LoginRsp rsp;
    req.set_name("czlvvv");
    SendMsgToSvr(cli, req, MsgCmd::LOGIN_REQ);

    unsigned int len = sizeof(recvbuff);
    if (cli.recv((void *)recvbuff, len, sizeof(MsgHeader)) < 0) {
        cout << cli.get_err_msg() << endl;
        return 0;
    }
    cout << "head:" << len << endl;
    MsgHeader *head = (MsgHeader *)recvbuff;
    int exp_len = ntohl(head->len);
    len = sizeof(recvbuff);
    if (cli.recv((void *)recvbuff, len, exp_len - sizeof(MsgHeader)) < 0) {
        cout << cli.get_err_msg() << endl;
        return 0;
    }
    std::string data(recvbuff, exp_len);
    rsp.ParseFromString(data);
    cout << rsp.DebugString() << endl;

    //ZoneSyn
    ZoneSynReq syn_req;
    ZoneSynRsp syn_rsp;
    syn_req.mutable_persion()->set_name("czlvvv");
    syn_req.mutable_persion()->mutable_point()->set_x(1);
    syn_req.mutable_persion()->mutable_point()->set_y(1);

    SendMsgToSvr(cli, syn_req, MsgCmd::ZONE_SYN_REQ);

    len = sizeof(recvbuff);
    if (cli.recv((void *)recvbuff, len, sizeof(MsgHeader)) < 0) {
        cout << cli.get_err_msg() << endl;
        return 0;
    }
    cout << "head:" << len << endl;
    head = (MsgHeader *)recvbuff;
    exp_len = ntohl(head->len);
    len = sizeof(recvbuff);
    if (cli.recv((void *)recvbuff, len, exp_len - sizeof(MsgHeader)) < 0) {
        cout << cli.get_err_msg() << endl;
        return 0;
    }
    std::string syndata(recvbuff, exp_len);
    syn_rsp.ParseFromString(syndata);
    cout << syn_rsp.DebugString() << endl;

    return 0;
}
