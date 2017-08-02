#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

#include "tcp_client.h"
#include "proto.h"

using namespace std;
using namespace proto;

char recvbuff[1024 * 1024 * 10];
unsigned int rv_len;

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

        usepos += nr;
    }

    return usepos;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);
    TcpClient cli;

    cli.init(argv[1], port);

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

        string snd;
        cmd.Serialize(snd);
        int ns = cli.send((void *)snd.c_str(), snd.size());
        if (ns < 0) {
            cout << cli.get_err_msg() << endl;
            return 0;
        }
    }

    return 0;
}
