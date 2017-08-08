//
// @Create by CaiZhili on 20170808
// @Brief player.cpp
//
#include <cstdlib>
#include <cstring>

#include "player.h"

Player::Player() : x_(0), y_(0) {
    name_[0] = '\0';
}

Player::~Player() {

}

void Player::set_name(const char *name) {
    if (!name)
        return;

    memset(name_, 0, sizeof(name_));
    strncpy(name_, name, sizeof(name_) - 1);
}

void Player::set_name(const char *name, const unsigned long size) {
    if (!name)
        return;

    unsigned long lenth = size;
    if (lenth > kNameMaxLen)
        lenth = kNameMaxLen;

    memcpy(name_, name, lenth);
}

const char *Player::name() {
    return name_;
}

void Player::set_x(int x) {
    x_ = x;
}

int Player::x() {
    return x;
}

void Player::set_y(int y) {
    y_ = y;
}

int Player::y() {
    return y_;
}
