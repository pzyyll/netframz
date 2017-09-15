#include <map>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>

using namespace std;

char sbuff[1024000];

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

const int MAXEVS = 1024;

int main(int argc, char **argv) {
    int fd = InitServers();
    if (fd < 0) {
        printf("err %d\n", fd);
    }

    HookSig();

    struct epoll_event ev, evs[MAXEVS];
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return -1;
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl");
        return -1;
    }

    for (;;) {
        int nds = epoll_wait(epoll_fd, evs, MAXEVS, 10);
        if (nds < 0) {
            perror("epoll_wait");
            return -1;
        }
        for (int i = 0; i < nds; ++i) {
            struct epoll_event *pev = evs + i;
            if (pev->data.fd == fd) {
                struct sockaddr_in addr;
                bzero(&addr, sizeof(addr));
                socklen_t len = sizeof(addr);
                int cli_fd = accept(fd, (struct sockaddr *)&addr, &len);
                if (cli_fd < 0) {
                    perror("accept");
                    return -1;
                }

                cout << "cli fd conn: " << cli_fd << endl;

                int sendbuff = 512;
                if (setsockopt(cli_fd, SOL_SOCKET, SO_SNDBUF, (void *)&sendbuff, sizeof(sendbuff)) < 0) {
                    perror("setsockopt");
                    return -1;
                }

                MakeFdBlockIs(false, cli_fd);
                struct epoll_event sev;
                sev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
                sev.data.fd = cli_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &sev) < 0) {
                    perror("epoll_ctl cli");
                    return -1;
                }
            } else {
                if (pev->events & EPOLLRDHUP) {
                    cout << "cli " << pev->data.fd << "is close." << endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, pev->data.fd, NULL);
                    close(pev->data.fd);
                    continue;
                }
                if (pev->events & EPOLLHUP) {
                    cout << "EPOLLHUP " << pev->data.fd << endl;
                    continue;
                }
                if (pev->events & EPOLLERR) {
                    cout << "EPOLLERR " << pev->data.fd << endl;
                    continue;
                }

                int i = 0;
                if (pev->events & EPOLLIN) {
                    char buff[1024] = {0};
                    int nr = read(pev->data.fd, buff, sizeof(buff));
                    cout << "cli " << pev->data.fd;
                    cout << ", read n=" << nr;
                    cout << ", " << buff << endl;

                    memset(sbuff, 'a', sizeof(sbuff));
                    int nw = write(pev->data.fd, sbuff, sizeof(sbuff));
                    if (nw < 0 && EAGAIN == errno) {
                        struct epoll_event mev;
                        mev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP;
                        mev.data.fd = pev->data.fd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pev->data.fd, &mev) < 0) {
                            perror("epoll_ctl mod");
                            return -1;
                        }
                        cout << "EPOLLIN -> EAGAIN" << endl;
                    }
                    cout << "EPOLLIN: ";
                    cout << nw << " bytes had send to cli " << pev->data.fd << endl;
                    ++i;
                }

                if (pev->events & EPOLLOUT) {
                    if (1 == i) {
                        cout << "EPOLLOUT: with EPOLLIN event" << endl;
                    } else {
                        char buff[1024];
                        memset(buff, 'a', sizeof(buff));
                        int nw = write(pev->data.fd, buff, sizeof(buff));
                        if (nw < 0 && EAGAIN == errno) {
                            cout << "EPOLLOUT -> EAGANIN" << endl;
                        }

                        cout << "EPOLLOUT: ";
                        cout << nw << " byes had send to cli " << pev->data.fd << endl;
                    }
/*
                    struct epoll_event mev;
                        mev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
                        mev.data.fd = pev->data.fd;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, pev->data.fd, &mev) < 0) {
                            perror("epoll_ctl mod");
                            return -1;
                        }
*/
                    ++i;
                }

                cout << "======== i cnt " << i << "=======" << endl;
            }
        }
    }

    return 0;
}
