#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <pthread.h>

using namespace std;

pthread_t threads[10240];
unsigned long cnt;

void *AnRobot(void *arg) {
    ++cnt;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int n = 0;
    while (cin >> n) {
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
