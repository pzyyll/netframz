#include <iostream>

#include "log_def.h"

using namespace std;

int main(int argc, char *argv[]) {
    InitLog(argv[0], 0, "./");
    LOG_INFO("abc%d", 12);
    LOG_WARN("WARNING");
    return 0;
}
