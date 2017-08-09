//
// @Create by CaiZhili on 20170808
// @Brief player.cpp
//
#include <cstdlib>
#include <cstring>

#include "player.h"

Player::Player() : conn_id_(0) x_(0), y_(0) {
    name_[0] = '\0';
}

Player::~Player() {

}

void Player::set_conn_id(unsigned long conn_id) {
    conn_id_ = conn_id;
}

unsigned long Player::conn_id() {
    return conn_id_;
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
        lenth = kNameMaxLen - 1;

    memcpy(name_, name, lenth);
    name_[lenth] = '\0';
}

std::string Player::name() {
    return std::string(name_);
}

void Player::set_x(int x) {
    x_ = x;
}

int Player::x() {
    return x_;
}

void Player::set_y(int y) {
    y_ = y;
}

int Player::y() {
    return y_;
}
