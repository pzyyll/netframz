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

char recvbuff[10240000];

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);
    TcpClient cli;

    cli.init(argv[1], port);

    int a;
    while (cin >> a) {
        if (0 == a) {
            unsigned int len = sizeof(recvbuff);
            cli.recv((void *)recvbuff, len);
            cout << "recv: " <<  len << endl;

            vector<string> vec_str;
            Cmd::Unpack(vec_str, recvbuff, len);
            for (int i = 0; (unsigned int)i < vec_str.size(); ++i) {
                cout << vec_str[i].size() << endl;
            }
            continue;
        }
        for (int i = 0; i < a; ++i) {
            string str(302400, 'a');
            string pack;
            Cmd::Packing(pack, str);
            int ns = cli.send((void *)pack.c_str(), pack.size());
            if (ns < 0) {
                cout << cli.get_err_msg() << endl;
                return 0;
            }
            cout << pack.size() << endl;
            sleep(1);
        }
    }

    return 0;
}
