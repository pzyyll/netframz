//
// @Create by CaiZhili on 20170808
// @Brief player.h
//

#include <string>

static const int kNameMaxLen = 256;

struct Point {
    Point(int xt = 0, int yt = 0) : x(xt), y(yt) {  }
    ~Point() {  }

    int x, y;
};

class Player {
public:
    Player();
    ~Player();

    void set_name(const std::string &name);
    std::string name();

    void set_conn_id(unsigned long conn_id);
    unsigned long conn_id();

    void set_last_act_time(unsigned long last_act_time);
    unsigned long last_act_time();

    void set_last_point(Point &last_point);
    Point &last_point();

    void set_point(Point point);
    Point &point();

    void set_scene_id(int scene_id);
    int scene_id();

private:
    char name_[kNameMaxLen];
    unsigned long conn_id_;
    unsigned long last_act_time_;
    Point last_point_;
    Point point_;
    int scene_id_;
};
