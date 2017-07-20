#include <iostream>
#include <cstdlib>
#include <string>

#include "tcp_client.h"

using namespace std;

char recvbuff[10240000];

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
            unsigned int len = sizeof(recvbuff);
            cli.recv((void *)recvbuff, len);
            recvbuff[len] = '\0';
            cout << "recv: " <<  len << endl;
            continue;
        }
        int n = 1024;
        //while (n--) {
            cli.send((void *)a.c_str(), a.size());
        //}
        cout << "send " << a << endl;
    }

    return 0;
}
