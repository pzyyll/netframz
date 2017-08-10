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
int SendToSvr(Cmd &cmd);
int RecvFromSvr();

void ProcessRspCmd(Cmd &cmd);
void ProcessLogin(const std::string &name);
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
        if ("recv" == a) {
            unsigned int len = sizeof(recvbuff) - rv_len;
            int ret = cli.recv((void *)(recvbuff + rv_len), len);
            if (ret < 0) {
                cout << cli.get_err_msg() << endl;
                continue;
            }
            cout << "recv: " <<  len << endl;
            rv_len += len;
            cout << "rv_len: " << rv_len << endl;

            int hn = BuffHandle(recvbuff, rv_len);
            if (hn < 0) {
                return 0;
            }
            rv_len -= hn;
            memmove(recvbuff, recvbuff + hn, rv_len);
            continue;
        }
        Cmd cmd;
        cmd.SetMsgData(a);
        cmd.SetType(0);

        string snd;
        cmd.Serialize(snd);
        cout << "snd: " << snd.size() << endl;
        int ns = cli.send((void *)snd.c_str(), snd.size());
        if (ns < 0) {
            cout << cli.get_err_msg() << endl;
            return 0;
        }
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

int SendToSvr(Cmd &cmd) {
    string snd;
    cmd.Serialize(snd);
    cout << "snd: " << snd.size() << endl;
    int ns = cli.send((void *)snd.c_str(), snd.size());
    if (ns < 0) {
        cout << cli.get_err_msg() << endl;
        return -1;
    }
    return 0;
}

int RecvFromSvr() {
    unsigned int len = sizeof(recvbuff) - rv_len;
    int ret = cli.recv((void *)(recvbuff + rv_len), len);
    if (ret < 0) {
        cout << cli.get_err_msg() << endl;
        continue;
    }
    cout << "recv: " <<  len << endl;
    rv_len += len;
    cout << "rv_len: " << rv_len << endl;

    int hn = BuffHandle(recvbuff, rv_len);
    if (hn < 0) {
        return 0;
    }
    rv_len -= hn;
    memmove(recvbuff, recvbuff + hn, rv_len);
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

    std::string str;
    req.SerializeToString(&str);

    Cmd cmd;
    cmd.SetType(MsgCmd::LOGIN_REQ);
    cmd.SetMsgData(str);

    SendToSvr(cmd);
}

void InputOption(const std::string &option) {
    //todo
}
