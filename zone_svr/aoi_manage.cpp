//
// @Created by czllo on 2017/8/17.
// @Brief
//
#include <cassert>

#include "aoi_manage.h"

inline void set_diff(const std::set<Vec2> &set1,
                     const std::set<Vec2> &set2,
                     std::set<Vec2> &res) {
    for (std::set<Vec2>::iterator itr = set1.begin(); itr != set1.end(); ++itr) {
        std::set<Vec2>::iterator find_itr = set2.find(*itr);
        if (find_itr == set2.end())
            res.insert(*itr);
    }
}

inline bool EqualVec2(const Vec2 &a, const Vec2 &b) {
    return (a.x == b.x && a.y == b.y);
}

inline Object *MakeObject(const unsigned long id, const Pos &pos) {
    Object *obj = new Object();
    assert(obj != NULL);

    obj->id = id;
    obj->pos = pos;

    return obj;
}

AOIManage::AOIManage() :
    grids_(NULL),
    map_width_(0), map_height_(0),
    grid_width_(0), grid_height_(0),
    is_init_(0) {

}

AOIManage::~AOIManage() {
    //todo release
    if (grids_) {
        delete [] grids_;
        for (auto itr: id2obj_map_)
            delete (itr.second);
        is_init_ = 0;
    }
}

int AOIManage::Init(unsigned int map_width, unsigned int map_height,
                    unsigned int grid_width, unsigned int grid_height) {
    map_width_ = map_width;
    map_height_ = map_width;
    grid_width_ = grid_width;
    grid_height_ = grid_height;

    grids_ = new AOIEntry[grid_width_ * grid_height_];
    assert(grids_ != NULL);

    is_init_ = 1;

    //avoid warnning
    return 0;
}


int AOIManage::AddPos(const unsigned long id,
                      const Pos &pos,
                      std::vector<unsigned long> &interest_ids) {
    if (!is_init_)
        return -1;

    Vec2 vec2 = CalcVec2(pos);

    if (!CheckVec2(vec2))
        return -1;

    if (FindObj(id))
        return -1;

    GetInterestUserIds(vec2, interest_ids);

    Object *obj = MakeObject(id, pos);
    id2obj_map_[id] = obj;
    AOIEntry &entry = grids_[IndexOf(vec2)];
    entry.obj_list.push_back(obj);

    return 0;
}

void AOIManage::DelPos(unsigned long id,
                       std::vector<unsigned long> &remove_ids) {
    if (!is_init_)
        return;

    Object *obj = FindObj(id);
    if (NULL == obj)
        return;

    Vec2 v = CalcVec2(obj->pos);
    AOIEntry &entry = grids_[IndexOf(v)];

    auto itr = find(entry.obj_list.begin(), entry.obj_list.end(), obj);
    if (itr != entry.obj_list.end())
        entry.obj_list.erase(itr);
    id2obj_map_.erase(id);

    delete obj;

    GetInterestUserIds(v, remove_ids);
}

void AOIManage::UpdatePos(const unsigned long id,
                          const Pos &pos,
                          std::vector<unsigned long> &remove_ids,
                          std::vector<unsigned long> &interest_ids) {
    if (!is_init_)
        return;

    Object *obj = FindObj(id);
    if (NULL == obj)
        return;

    Vec2 nvec2 = CalcVec2(pos);

    if (!CheckVec2(nvec2))
        return;

    Vec2 ovec2 = CalcVec2(obj->pos);

    //Grid position no change
    if (EqualVec2(ovec2, nvec2)) {
        obj->pos = pos;
        GetInterestUserIds(nvec2, interest_ids);
        return;
    }

    AOIEntry &oentry = grids_[IndexOf(ovec2)];
    auto itr = std::find(oentry.obj_list.begin(), oentry.obj_list.end(), obj);
    if (itr != oentry.obj_list.end())
        oentry.obj_list.erase(itr);

    std::set<Vec2> oset, nset, diff_set;

    CalcGridSet(ovec2, oset);
    CalcGridSet(nvec2, nset);

    //diff_set = oset - nset;
    set_diff(oset, nset, diff_set);

    GetUserIdsFromGrid(diff_set, remove_ids);
    GetUserIdsFromGrid(nset, interest_ids);

    obj->pos = pos;
    AOIEntry &nentry = grids_[IndexOf(nvec2)];
    nentry.obj_list.push_back(obj);
}

Vec2 AOIManage::CalcVec2(const Pos &pos) {
    Vec2 vec2;
    vec2.x = pos.x / (int)ceil(map_width_ * 1.0 / grid_width_);
    vec2.y = pos.y / (int)ceil(map_height_ * 1.0 / grid_height_);

    return vec2;
}

unsigned int AOIManage::IndexOf(const Vec2 &vec2) {
    return (vec2.x * grid_width_ + vec2.y);
}

void AOIManage::CalcGridSet(const Vec2 &vec2, std::set<Vec2> &vec2_set) {
    Vec2 ovec2;
    ovec2.x = vec2.x - 1;
    ovec2.y = vec2.y - 1;

    const int kStep = 3;
    for (int i = 0; i < kStep; ++i) {
        for (int j = 0; j < kStep; ++j) {
            Vec2 vec2;
            vec2.x = ovec2.x + i;
            vec2.y = ovec2.y + j;

            if (!CheckVec2(vec2))
                continue;

            vec2_set.insert(vec2);
        }
    }
}

bool AOIManage::CheckVec2(const Vec2 &vec2) {
    if (vec2.x < 0 || vec2.y < 0)
        return false;
    if ((unsigned)vec2.x >= grid_width_ || (unsigned)vec2.y >= grid_height_)
        return false;

    return true;
}

Object *AOIManage::FindObj(const unsigned long id) {
    Id2ObjMap::iterator find_itr = id2obj_map_.find(id);
    if (find_itr != id2obj_map_.end())
        return find_itr->second;
    return NULL;
}

void AOIManage::GetUserIdsFromGrid(const std::set<Vec2> &vec2_set,
                                   std::vector<unsigned long> &ids) {
    for (auto vec2: vec2_set) {
        AOIEntry &entry = grids_[IndexOf(vec2)];
        for (auto obj: entry.obj_list)
            ids.push_back(obj->id);
    }
}

void AOIManage::GetInterestUserIds(const Vec2 &vec2,
                                   std::vector<unsigned long> &ids) {
    std::set<Vec2> vec2_set;
    CalcGridSet(vec2, vec2_set);
    GetUserIdsFromGrid(vec2_set, ids);
}
