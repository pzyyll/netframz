#include <map>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>

#include "nf_event.h"
#include "nf_event_iotask.h"
#include "nf_event_timer_task.h"

using namespace std;

int MakeFdBlockIs(bool is_block, int fd) {
    int val = fcntl(fd, F_GETFL, 0);
    if (val < 0) {
        cout << strerror(errno) << endl;
        return -1;
    }

    if (is_block) {
        val &= ~O_NONBLOCK;
    } else {
        val |= O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, val) < 0) {
        cout << strerror(errno) << endl;
        return -1;
    }

    return 0;
}

static void HookSig() {
    struct sigaction sigact;
    bzero(&sigact, sizeof(sigact));
    sigact.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sigact, NULL);
}

static int InitServers() {
    int ret = 0;
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int reused = 1;
    ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reused, sizeof(reused));
    if (ret < 0)
        return -1;

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3399);
    addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
        return ret;

    ret = listen(listen_fd, 1024);
    if (ret < 0)
        return -1;

    return listen_fd;
}

void read_cb(EventLoop *loop, task_data_t data, int mask) {
    int cli_fd = data.data.fd;
    cout <<  "hah" << endl;

    //MakeFdBlockIs(false, cli_fd);

    char buff[1024] = {0};
    int nr = read(cli_fd, buff, sizeof(buff));
    if (nr < 0) {
        cout << strerror(errno) << endl;
        return ;
    } else if (nr == 0) {
        cout << "cli close" << endl;
        return ;
    }
    cout << buff << endl;

    char str[] = "abc\n";
    int n = ::write(cli_fd, str, 0);
    if (n < 0) {
        cout << "errno : " << errno << " msg: ";
        cout << strerror(errno) << endl;
    } else if (n == 0) {
        cout << "n = 0" << endl;
    }

    cout << "end" << endl;
}

void write_cb(EventLoop *loop, task_data_t *task, int mask) {

}

void accept_cb(EventLoop *loop, task_data_t data, int mask) {
    IOTask &task = *((IOTask *)data.data.ptr);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int cli_fd = accept(task.GetFd(), (struct sockaddr *)&addr, &len);
    if (cli_fd < 0) {
        printf ("accept fail cli_fd %d\n", cli_fd);
        return;
    }

    printf("cli fd : %d \n", cli_fd);
    IOTask *cli_task = new IOTask(*loop, cli_fd, EVSTAT::EV_READABLE);
    task_data_t iodata = { .data = { .fd = cli_fd } };
    cli_task->SetPrivateData(iodata);
    cli_task->Bind(read_cb);
    cli_task->Start();
}

int main(int argc, char **argv) {
    int fd = InitServers();
    if (fd < 0) {
        printf("err %d\n", fd);
    }

    HookSig();

    EventLoop loop;
    IOTask accept_task(loop, fd, EVSTAT::EV_READABLE);
    task_data_t data = { .data = { .ptr = &accept_task } };
    accept_task.SetPrivateData(data);
    accept_task.Bind(accept_cb);
    accept_task.Start();

    loop.Run();
    return 0;
}
