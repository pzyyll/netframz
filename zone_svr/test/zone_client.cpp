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

Persion persion;
map<std::string, Persion> persions_map;
int login_success = 0;
int is_exit = 1;
pthread_t recv_thread;

struct Pos {
    int x;
    int y;
};

void *recv_hld(void *args);

int recv_wait;
int RecvWait(int sec);

int BuffHandle(const char *buff, const unsigned int len);
int SendMsgToSvr(TcpClient &cli, const ::google::protobuf::Message &msg, const unsigned int type);
int RecvFromSvr(TcpClient &cli, char *buff, unsigned int &len);

int Login();
int ProcessLogin(const std::string &name);
void ProcessMove(Pos mv_direct);
//void ProcessChat(const std::string &content);

void ProcessRspCmd(Cmd &cmd);
void ProcessLoginRsp(const std::string &data);
//void ProcessZoneSynRsp(const std::string &data);
//void ProcessChatRsp(const std::string &data);
//void ProcessZoneSyn(const std::string &data);
//void ProcessChatStat(const std::string &data);

void PrintStat();
int InputOption();

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);

    if (cli.init(argv[1], port, -1, -1) < 0) {
        cout << "Tcp client init fail." << endl;
        return 0;
    }

    if (pthread_create(&recv_thread, NULL, recv_hld, NULL) != 0) {
        cout << "Create recv hld fail." << endl;
        return 0;
    }

    if (Login() < 0) {
        return 0;
    }

    PrintStat();

    while (true) {
        if (is_exit || InputOption() < 0)
            break;
    }

    return 0;
}

void *recv_hld(void *args) {
    while (true) {
        if (RecvFromSvr(cli, recvbuff, rv_len) < 0) {
            is_exit = 1;
            break;
        }

        int hn = BuffHandle(recvbuff, rv_len);
        if (hn < 0) {
            is_exit = 1;
            break;
        }

        rv_len -= hn;
        memmove(recvbuff, recvbuff + hn, rv_len);
    }

    pthread_exit(NULL);
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
            ProcessLoginRsp(cmd.GetMsgData());
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

int Login() {
    cout << "entry: " << endl;
    std::string name;
    cin >> name;

    return ProcessLogin(name);
}

int ProcessLogin(const std::string &name) {
    LoginReq req;
    req.set_name(name);

    SendMsgToSvr(cli, req, MsgCmd::LOGIN_REQ);

    if (RecvWait(5) < 0) {
        cout << "time out." << endl;
        return -1;
    }
    return 0;
}

void ProcessMove(Pos mv_direct) {

}


int InputOption() {
    //todo
    int ret = 0;
    std::string option;

    std::cin >> option;
    if (option.size() != 1) {
        cout << "Option invalid." << endl;
        return ret;
    }

    struct Pos pos = { 0, 0 };
    switch (option[0]) {
        case 'h':
            pos.x = -1;
            ProcessMove(pos);
            break;
        case 'i':
            pos.x = +1;
            ProcessMove(pos);
            break;
        case 'j':
            pos.y = +1;
            ProcessMove(pos);
            break;
        case 'k':
            pos.y = -1;
            ProcessMove(pos);
            break;
        case 'c':
            break;
        case 'q':
            ret = -1;
            break;
        default:
            break;
    }

    return ret;
}

#define TIME_INTERVAL 1000
int RecvWait(int sec) {
    recv_wait = 1;
    int usec = 0;
    while (recv_wait) {
        usleep(TIME_INTERVAL);
        usec += TIME_INTERVAL;
        if (sec * 1000000 <= usec)
            break;
    }

    if (recv_wait)
        return -1;

    return 0;
}

void ProcessLoginRsp(const std::string &data) {
    LoginRsp rsp;
    if (!rsp.ParseFromString(data)) {
        return;
    }

    if (rsp.ret() != MsgRet::SUCCESS) {
        cout << rsp.err_msg() << endl;
        return;
    }

    for (int i = 0; i < rsp.zone_stat().persion_list_size(); ++i) {
        const Persion &persion = rsp.zone_stat().persion_list(i);
        persions_map[persion.name()] = persion;
    }

    recv_wait = 0;
    is_exit = 0;
}

void PrintStat() {
    printf("All online:\n");
    printf("%s(%d,%d)(*)\n", persion.name().c_str(),
                            persion.point().x(),
                            persion.point().y());

    map<std::string, Persion>::iterator itr;
    for (itr = persions_map.begin(); itr != persions_map.end(); ++itr) {
        Persion &per = itr->second;
        printf("%s(%d, %d)\n", per.name().c_str(), per.point().x(), per.point().y());
    }

    printf("Chat:\n");
}
