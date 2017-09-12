//
// @Create by CaiZhili on 20170808
// @Brief player.cpp
//

#include <cstdlib>
#include <cstring>

#include "player.h"

Player::Player() : conn_id_(0), last_act_time_(0), last_point_(), point_() {
    name_[0] = '\0';
}

Player::~Player() {

}

void Player::set_name(const std::string &name) {
    unsigned long lenth = name.size();
    if (lenth > kNameMaxLen)
        lenth = kNameMaxLen - 1;

    memcpy(name_, name.c_str(), lenth);
    name_[lenth] = '\0';
}

std::string Player::name() {
    return std::string(name_);
}

void Player::set_conn_id(unsigned long conn_id) {
    conn_id_ = conn_id;
}

unsigned long Player::conn_id() {
    return conn_id_;
}

void Player::set_last_act_time(unsigned long last_act_time) {
    last_act_time_ = last_act_time;
}

unsigned long Player::last_act_time() {
    return last_act_time_;
}

void Player::set_last_point(Point &last_point) {
    last_point_ = last_point;
}

Point &Player::last_point() {
    return last_point_;
}

void Player::set_point(Point point) {
    point_ = point;
}

Point &Player::point() {
    return point_;
}

void Player::set_scene_id(int scene_id) {
    scene_id_ = scene_id;
}

int Player::scene_id() {
    return scene_id_;
}
