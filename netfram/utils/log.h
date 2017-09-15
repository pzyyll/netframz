#ifndef NETFRAMZ_SRC_LOG_H_
#define NETFRAMZ_SRC_LOG_H_

#include <glog/logging.h>

#define BEGINLOG \
    google::InitGoogleLogging("Debug"); \
    FLAGS_log_dir = "./";

//LOG(INFO) << ...;

#endif //NETFRAMZ_SRC_LOG_H_

