//
// @Create by CaiZhili on 20170808
// @Brief player.h
//

#include <string>

static const int kNameMaxLen = 256;

class Player {
public:
    Player();
    ~Player();

    void set_conn_id(unsigned long conn_id);
    unsigned long conn_id();

    void set_name(const char *name);
    void set_name(const char *name, const unsigned long size);
    std::string char *name();

    void set_x(int x);
    int x();

    void set_y(int y);
    int y();

private:
    unsigned long conn_id_;
    char name_[kNameMaxLen];
    int x_, y_;
};
