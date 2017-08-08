//
// @Create by CaiZhili on 20170808
// @Brief player.h
//

static const int kNameMaxLen = 256;

class Player {
public:
    Player();
    ~Player();

    void set_name(const char *name);
    void set_name(const char *name, const unsigned long size);
    const char *name();

    void set_x(int x);
    int x();

    void set_y(int y);
    int y();

private:
    char name_[kNameMaxLen];
    int x_, y_;
};
