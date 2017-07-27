#ifndef TEST_PROTO_H
#define TEST_PROTO_H

namespace proto {
    struct Head {
        int len;
    };

    class Cmd {
    public:
        Cmd() : size_(0), container_(NULL);
        ~Cmd() {
            if (container_) {
                delete container_;
                container_ = NULL;
            }
        }

    private:
        unsigned long size_;
        char *container_;
    };
}

#endif //TEST_PROTO_H
