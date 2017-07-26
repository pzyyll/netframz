#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

#include "tcp_client.h"

using namespace std;

char recvbuff[10240000];
char sendbuff[10240];
int port;
string ip;

void AnRobot() {
    char rv_buff[10240];
    char snd_buff[1024];
    TcpClient cli;
    cli.init(ip.c_str(), port);

    memset(snd_buff, 'a', sizeof(snd_buff));

    int ns = cli.send((void *)sendbuff, sizeof(sendbuff));
    if (ns < 0)
        return 0;
    cout << "send size = " << ns << endl;

    unsigned int len = sizeof(recvbuff);
    cli.recv((void *)recvbuff, len);
    recvbuff[len] = '\0';
    cout << "recv: " <<  len << endl;
    //cout << "str: " << recvbuff << endl;

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }
    port = atoi(argv[2]);
    ip = string(argv[1]);

    AnRobot();

    return 0;
}
