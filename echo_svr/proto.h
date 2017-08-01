#ifndef TEST_PROTO_H
#define TEST_PROTO_H

#include <string>
#include <vector>

namespace proto {
    struct Head {
        int len;
    };

    class Cmd {
    public:
        static void Packing(std::string &pack, const std::string &str) {
            struct Head head;
            head.len = htonl(str.size());
            pack.append((char *) &head, sizeof(head));
            pack.append(str);
        }

        static void Unpack(std::vector<std::string> &vec_str, const char *buf, unsigned long size) {
            if (NULL == buf)
                return;

            int fpos = 0;
            while ((unsigned int)fpos < size) {
                if (size - fpos < sizeof(struct Head)) {
                    break;
                }
                struct Head *head = (struct Head *)(buf + fpos);
                int len = ntohl(head->len);
                fpos += sizeof(struct Head);
                if (size - fpos < (unsigned long) len) {
                    break;
                }
                std::string item(buf + fpos, len);
                vec_str.push_back(item);
                fpos += len;
            }
        }
    };
}

#endif //TEST_PROTO_H
