#include <map>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "nf_event.h"
#include "nf_event_task.h"
#include "nf_event_timer_task.h"

using namespace std;

struct buffer {
    char *buf;
    unsigned long size;
};

struct TimerPriData {
    int fd;
    TimerTask *timer;
};

//s
#define OUT_TIME_LIMIT 10

map<int, IOTask*> task_map;
map<int, unsigned long> last_time_map;

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

void read_cb(EventLoop &loop, IOTask &task, int mask) {
    int cli_fd = task.get_fd();
    if (mask & EVSTAT::EV_RDHUP) {
        cout << "cli exit: " << cli_fd << endl;
        task.Stop();
        task_map.erase(cli_fd);
        return;
    }

    char buff[1024];
    int rv = read(cli_fd, buff, sizeof(buff));
    if (rv < 0) {
        cout << "read err " << cli_fd << endl;
        return;
    }

    cout << "mask : " << mask << endl;

    buff[rv] = '\0';
    cout << "rv from " << cli_fd << ": " << buff << endl;

    char msg[1024];
    snprintf(msg, sizeof(msg), "cli %d echo: %s\n", cli_fd, buff);
    write(cli_fd, msg, strlen(msg));

    //update last active time
    struct timeval now;
    gettimeofday(&now, NULL);
    last_time_map[cli_fd] = now.tv_sec;
}

void write_cb(EventLoop &loop, IOTask &task, int mask) {

}

void tick_cb(EventLoop *loop, task_data_t data, int mask) {
    TimerPriData *sdata = (TimerPriData *)data.data.ptr;
    int fd = sdata->fd;
    TimerTask *timer = sdata->timer;
    unsigned long last_act = last_time_map[fd];

    struct timeval now;
    gettimeofday(&now, NULL);
    if ((unsigned long)now.tv_sec - last_act > OUT_TIME_LIMIT) {
        map<int, IOTask*>::iterator find = task_map.find(fd);
        if (find == task_map.end()) {
            cout << "cli maybe exit not find task for " << fd << endl;
            return;
        }

        IOTask *task = find->second;
        task->Stop();
        close(fd);
        task_map.erase(fd);
        delete task;
        task = NULL;
        delete sdata;
        sdata = NULL;
        delete timer;
        timer = NULL;

        cout << "close cli " << fd << endl;
        return;
    }

    timer->Restart();
}

void accept_cb(EventLoop &loop, IOTask &task, int mask) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int cli_fd = accept(task.get_fd(), (struct sockaddr *)&addr, &len);
    if (cli_fd < 0) {
        printf ("accept fail cli_fd %d\n", cli_fd);
        return;
    }

    printf("cli fd : %d \n", cli_fd);
    IOTask *cli_task = new IOTask(loop, cli_fd, EVSTAT::EV_READABLE);
    cli_task->Bind(read_cb);
    cli_task->Start();
    task_map[cli_fd] = cli_task;

    struct timeval now;
    gettimeofday(&now, NULL);
    last_time_map[cli_fd] = now.tv_sec;

    TimerTask *timer = new TimerTask(loop, 10000, 0);
    timer->Bind(tick_cb);
    TimerPriData *sdata = new TimerPriData;
    sdata->fd = cli_fd;
    sdata->timer = timer;
    task_data_t data;
    data.data.ptr = sdata;
    timer->SetPrivateData(data);
    timer->Start();
}

int main(int argc, char **argv) {
    int fd = InitServers();
    if (fd < 0) {
        printf("err %d\n", fd);
    }

    HookSig();


    EventLoop loop;
    IOTask accept_task(loop, fd, EVSTAT::EV_READABLE);
    accept_task.Bind(accept_cb);
    accept_task.Start();

    loop.Run();
    return 0;
}
