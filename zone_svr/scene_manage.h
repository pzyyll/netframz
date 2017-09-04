//
// @Create by Caizhili on 20170904
// @Brief
//

#ifndef NF_ZONESVR_SCENE_MANAGE
#define NF_ZONESVR_SCENE_MANAGE

#include <vector>
#include "aoi_manage.h"

//TODO

class SceneManage {
    typedef std::vector<AOIManage *> VecAOI;
public:
    SceneManage();

    ~SceneManage();

    AOIManage *AddAOIManager(Rect &map_size, Rect &grid_size);

    AOIManage *GetAOIAt(unsigned index);

    unsigned int GetSize();

private:
    VecAOI vec_aoi_;
};

#endif
