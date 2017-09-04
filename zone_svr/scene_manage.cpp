//
// @Create by Caizhili on 20170904
// @Brief
//

#include <cassert>
#include "scene_manage.h"

//TODO

SceneManage::SceneManage() {

}

SceneManage::~SceneManage() {
    for (int i = 0; i < vec_aoi_.size(); ++i) {
        delete vec_aoi_[i];
    }

    vec_aoi_.clear();
}

AOIManage *SceneManage::AddAOIManager(Rect &map_size, Rect &grid_size) {
    AOIManage *aoi_new = new AOIManage;
    assert(aoi_new != NULL);

    aoi_new->Init(map_size, grid_size);

    vec_aoi_.push_back(aoi_new);

    return aoi_new;
}

unsigned int SceneManage::GetSize() {
    return vec_aoi_.size();
}
