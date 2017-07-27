#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "tcp_client.h"
#include "proto.h"

using namespace std;
using namespace proto;

pthread_t threads[10240];
int port;
string ip;

unsigned long cnt;

void *AnRobot(void *arg) {
    char rv_buff[10240];
    TcpClient cli;
    if (cli.init(ip.c_str(), port, 1000, 5000) < 0) {
        cout << "init fail : " << cli.get_err_msg() << endl;
    }

    string str(100, 'a');
    string pack;
    Cmd::Packing(str, pack);

    int ns = cli.send((void *)pack.c_str(), pack.size());
    if (ns < 0) {
        cout << "send fail : " <<  cli.get_err_msg() << endl;
        pthread_exit(NULL);
    }

    unsigned int len = sizeof(rv_buff);
    int ret = cli.recv((void *)rv_buff, len);
    if (ret < 0) {
        cout << "rv fail: " << cli.get_err_msg() << endl;
        pthread_exit(NULL);
    }

    vector<string> str_vec;
    Cmd::Unpack(str_vec, rv_buff, len);
    if (str_vec.size() >= 1) {
        //cout << str_vec[0] << endl;
        ++cnt;
    }

    pthread_exit(NULL);
}

void StartNRobot(int n) {
        struct timeval before_time;
        gettimeofday(&before_time, NULL);
        cnt = 0;
        for (int i = 0; i < n; ++i) {
            pthread_create(threads + i, NULL, AnRobot, NULL);
        }

        for (int i = 0; i < n; ++i) {
            pthread_join(threads[i], NULL);
        }

        struct timeval end_time;
        gettimeofday(&end_time, NULL);
        unsigned long total_ms = (end_time.tv_sec - before_time.tv_sec) * 1000
            + (end_time.tv_usec - before_time.tv_usec) / 1000;
        cout << "==========" << endl;
        cout << "Total " << n;
        cout << ", Success " << cnt;
        cout << ", Time " << total_ms << "ms";
        cout << ", PerTime " << ((double)total_ms / cnt) << "ms" << endl;
        cout << "==========" << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }
    port = atoi(argv[2]);
    ip = string(argv[1]);

    int n = 0, n2 = 0;
    while (cin >> n) {
        if (0 == n) {
            cin >> n2;
            while (true) {
                StartNRobot(n2);
                sleep(1);
            }
        }

        StartNRobot(n);
    }

    return 0;
}
