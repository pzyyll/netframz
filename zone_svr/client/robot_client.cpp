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
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int port;
string ip;

unsigned long cnt;
unsigned long nmsg = 1;
unsigned long pkg_len = 100;

#define LOOP_N_BEGING(n) for (unsigned int i = 0; i < (unsigned int)(n); ++i) {
#define LOOP_N_END }
#define RV_MAX_SIZE (2048 * 1024)

void *AnRobot(void *arg) {
    (void)(arg);
    char *rv_buff = new char[RV_MAX_SIZE];
    if (!rv_buff) pthread_exit(NULL);

    TcpClient cli;
    if (cli.init(ip.c_str(), port, 5000, 5000) < 0) {
        cout << "init fail : " << cli.get_err_msg() << endl;
    }

LOOP_N_BEGING(nmsg)
    Cmd cmd;
    cmd.SetMsgData(string(pkg_len, 'a'));
    string pack;
    cmd.SerializeTo(pack);

    int ns = cli.send((void *)pack.c_str(), pack.size());
    if (ns < 0) {
        cout << "send fail : " <<  cli.get_err_msg() << endl;
        pthread_exit(NULL);
    }

    unsigned int ex_len = sizeof(struct MsgHeader);
    unsigned int nlen = 0;
    int ret = cli.recv((void *)rv_buff, nlen, ex_len);
    if (ret < 0) {
        cout << "rv fail: " << cli.get_err_msg() << endl;
        cli.reconnect();
        continue;
    }

    struct MsgHeader *head = (struct MsgHeader *)(rv_buff);
    unsigned int msg_len = ntohl(head->len);
    if (MAX_CMD_LEN < msg_len || msg_len < sizeof(struct MsgHeader)) {
        cout << "pkg len err " << msg_len << endl;
        cli.reconnect();
        continue;
    }

    ex_len = msg_len - sizeof(struct MsgHeader);
    ret = cli.recv((void *)rv_buff, nlen, ex_len);
    if (ret < 0) {
        cout << "rv fail: " << cli.get_err_msg() << endl;
        cli.reconnect();
        continue;
    }

    if (nlen == cmd.GetMsgData().size()) {
        pthread_mutex_lock(&lock);
        cnt++;
        pthread_mutex_unlock(&lock);
    }
LOOP_N_END

    delete rv_buff;
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
        cout << endl;
        cout << "Total " << n * nmsg;
        cout << "(" << n << "x" << nmsg << "msg)";
        cout << ", Success " << cnt;
        cout << ", Time " << total_ms << "ms";
        cout << ", PerTime " << ((double)total_ms / cnt) << "ms" << endl;
        cout << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout << "exe ip port" << endl;
        return -1;
    }
    port = atoi(argv[2]);
    ip = string(argv[1]);

    int n = 0, n2 = 0;
    cout << "option(0:loop, 1: once) client_nums msg_nums per-msg-len" << endl;
    while (cin >> n) {
        cin >> n2 >> nmsg >> pkg_len;
        switch (n) {
        case 0:
            while (true) {
                StartNRobot(n2);
                sleep(1);
            }
            break;
        case 1:
            StartNRobot(n2);
        default:
            break;
        }
        cout << "option(0:loop, 1: once) client_nums msg_nums per-msg-len" << endl;
    }

    return 0;
}
