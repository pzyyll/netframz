#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

#include "tcp_client.h"

using namespace std;

char recvbuff[10240000];
char sendbuff[10240];

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }

    int port = atoi(argv[2]);
    TcpClient cli;

    cli.init(argv[1], port);

    memset(sendbuff, 'a', sizeof(sendbuff));

    //string a;
    int a;
    while (cin >> a) {
        if (0 == a) {
            unsigned int len = sizeof(recvbuff);
            cli.recv((void *)recvbuff, len);
            recvbuff[len] = '\0';
            cout << "recv: " <<  len << endl;
            //cout << "str: " << recvbuff << endl;
            continue;
        }
        int n = a;
        int ns = 0;
        while (n--) {
            ns = cli.send((void *)sendbuff, sizeof(sendbuff));
            if (ns < 0)
                return 0;
            cout << "send size = " << ns << endl;
            //sleep(1);
        }
        //cout << "send " << a << endl;
    }

    return 0;
}
