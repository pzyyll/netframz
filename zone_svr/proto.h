#ifndef TEST_PROTO_H
#define TEST_PROTO_H

#include <string>
#include <vector>
#include <cstddef>

namespace proto {
#pragma pack(1)
struct MsgHeader {
    unsigned int len;
    unsigned int type;
};
#pragma pack()

#define MAX_CMD_LEN (1024 * 1024)

enum MsgType {
    ZONE_LOGING = 0X0001,
    ZONE_STAT   = 0x1001,
    ZONE_CHAT   = 0x1002,
};

class Cmd {
public:
    Cmd() { }

    ~Cmd() { }

    int Parse(const char *data, unsigned long lenth) {
        unsigned int head_len = sizeof(struct MsgHeader);
        if (lenth < head_len) {
            return 0;
        }

        const struct MsgHeader *head = (const struct MsgHeader *)(data);
        unsigned int msg_len = ntohl(head->len);
        unsigned int type = ntohl(head->type);

        if (msg_len > MAX_CMD_LEN) {
            snprintf(err_, sizeof(err_), "pkg is too long. msg len|%u", msg_len);
            return -1;
        }

        if (msg_len < head_len) {
            snprintf(err_, sizeof(err_), "parse msg len err. msg len|%u", msg_len);
            return -1;
        }

        if (msg_len > lenth) {
            snprintf(err_, sizeof(err_), "msg is incomplete. msg_len|%u, buf len|%lu", msg_len, lenth);
            return 0;
        }

        SetType(type);
        SetMsgData(std::string(data + head_len, msg_len - head_len));
        return (int)msg_len;
    }

    void Serialize(std::string &data) {
        data.clear();
        struct MsgHeader head;
        head.len = htonl(sizeof(head) + msg_data_.size());
        head.type = htonl(type_);
        data.append((char *) &head, sizeof(head));
        data.append(msg_data_);
    }

    std::string GetErr() { return std::string(err_); }

    unsigned long GetType() { return type_; }

    void SetType(unsigned int type) { type_ = type; }

    std::string &GetMsgData() { return msg_data_; }

    void SetMsgData(const std::string &msg_data) { msg_data_ = msg_data; }

protected:
    unsigned int type_;
    std::string msg_data_;
    char err_[256];
};

}

#endif //TEST_PROTO_H
