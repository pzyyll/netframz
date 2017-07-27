#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <pthread.h>

#include "tcp_client.h"

using namespace std;

pthread_t threads[10240];
int port;
string ip;

unsigned long cnt;

void *AnRobot(void *arg) {
    char rv_buff[10240];
    char snd_buff[1024];
    TcpClient cli;
    if (cli.init(ip.c_str(), port, 1000, 5000) < 0) {
        cout << "init fail : ";
        cout << cli.get_err_msg() << endl;
    }

    memset(snd_buff, 'a', sizeof(snd_buff));

    int ns = cli.send((void *)snd_buff, sizeof(snd_buff));
    if (ns < 0) {
        cout << "send fail : " << endl;
        cout << cli.get_err_msg() << endl;
        return 0;
    }
    //cout << "send size = " << sizeof(snd_buff) << endl;

    unsigned int len = sizeof(rv_buff);
    cli.recv((void *)rv_buff, len);
    rv_buff[len] = '\0';
    //cout << "recv: " <<  len << endl;
    if (len - sizeof(snd_buff) == 0) {
        ++cnt;
    }
    else {
        cout << cli.get_err_msg() << endl;
    }
    //cout << "str: " << recvbuff << endl;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }
    port = atoi(argv[2]);
    ip = string(argv[1]);

    int n = 0;
    while (cin >> n) {
        if (0 == n) {
            while (true) {
                cnt = 0;
                int n = 200;
                for (int i = 0; i < n; ++i) {
                    pthread_create(threads + i, NULL, AnRobot, NULL);
                }

                for (int i = 0; i < n; ++i) {
                    pthread_join(threads[i], NULL);
                }

                cout << "==========" << endl;
                cout << cnt << endl;
                cout << "==========" << endl;
                sleep(1);
            }
        }
        cnt = 0;
        for (int i = 0; i < n; ++i) {
            pthread_create(threads + i, NULL, AnRobot, NULL);
        }

        for (int i = 0; i < n; ++i) {
            pthread_join(threads[i], NULL);
        }

        cout << "==========" << endl;
        cout << cnt << endl;
        cout << "==========" << endl;
    }

    return 0;
}
