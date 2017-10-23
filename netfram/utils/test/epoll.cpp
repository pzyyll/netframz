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
#include "nf_socket_api.h"

using namespace std;
using namespace nf;

typedef map<unsigned long, IOTask *> TaskMap;
TaskMap task_map;

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

const unsigned kMaxBuffLen = 10240000;
char send_buff[kMaxBuffLen];
unsigned buf_fpos, buf_tpos;

void read_cb(EventService *es, task_data_t data, int mask) {
    int cli_fd = data.data.fd;
    IOTask *cli_task = task_map[cli_fd];
    cout <<  "hah" << endl;


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

    if (string(buff) == "send") {
        cli_task->AddMask(EVSTAT::EV_WRITEABLE);
        cli_task->Restart();
        return;
    }


    string appstr(102400, buff[0]);
    if (buf_tpos + appstr.size() >= kMaxBuffLen) {
        cout << "buff full." << endl;
        return ;
    }

    memcpy(send_buff + buf_tpos, appstr.c_str(), appstr.size());
    buf_tpos += appstr.size();

    cout << "contain size : " << (buf_tpos - buf_fpos) << endl;
}

void write_cb(EventService *es, task_data_t data, int mask) {
    int cli_fd = data.data.fd;
    IOTask *cli_task = task_map[cli_fd];

    unsigned send_size = buf_tpos - buf_fpos;
    int nw = Writen(cli_fd, send_buff + buf_fpos, send_size);
    if (nw < 0) {
        perror("write err ");
    }

    if (nw != send_size && EAGAIN == errno) {
        perror("write err ");
    }

    cout << "WRITEABLE: ";
    cout << nw << " bytes hads sent to cli." << endl;
    buf_fpos += nw;
    if (buf_fpos == buf_tpos) {
        buf_fpos = buf_tpos = 0;
        cli_task->RemoveMask(EVSTAT::EV_WRITEABLE);
        cli_task->Restart();
        cout << "complete write" << endl;
    }

    cout << "remain size : " << (buf_tpos - buf_fpos) << endl;
}

void accept_cb(EventService *es, task_data_t data, int mask) {
    IOTask &task = *((IOTask *)data.data.ptr);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int cli_fd = accept(task.GetFd(), (struct sockaddr *)&addr, &len);
    if (cli_fd < 0) {
        printf ("accept fail cli_fd %d\n", cli_fd);
        return;
    }

    int sendbuff = 512;
    if (setsockopt(cli_fd, SOL_SOCKET, SO_SNDBUF, (void *)&sendbuff, sizeof(sendbuff)) < 0) {
        perror("setsockopt");
        return;
    }

    MakeFdBlockIs(false, cli_fd);

    printf("cli fd : %d \n", cli_fd);
    IOTask *cli_task = new IOTask(*es, cli_fd, EVSTAT::EV_READABLE);
    task_data_t iodata = { .data = { .fd = cli_fd } };
    cli_task->SetPrivateData(iodata);
    cli_task->SetReadHandle(read_cb);
    cli_task->SetWriteHandle(write_cb);
    cli_task->Start();

    task_map[cli_fd] = cli_task;
}

int main(int argc, char **argv) {
    int fd = InitServers();
    if (fd < 0) {
        perror("err ");
        printf("err %d\n", fd);
        return -1;
    }
    cout << "listend " << fd << endl;

    HookSig();

    EventService es;
    es.Init();
    IOTask accept_task(es, fd, EVSTAT::EV_READABLE);
    task_data_t data = { .data = { .ptr = &accept_task } };
    accept_task.SetPrivateData(data);
    accept_task.Bind(accept_cb);
    accept_task.Start();

    es.Run();
    return 0;
}
