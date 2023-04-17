#pragma once
#include "sharedstruct.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "descriptorheap.h"
#include "constantbuffer.h"
#include <DirectXMath.h>

class Shooting {
public:
    // 弾を撃つ
    bool Shoot(
        std::vector<Mesh> &object,
        std::vector<OnSceneObject> &on_scene_objects,
        DirectX::XMFLOAT3 targetposition,
        float position_x,
        float position_y,
        float position_z,
        float scale_x = 1.0f,
        float scale_y = 1.0f,
        float scale_z = 1.0f);
    // 弾を動かす
    bool MoveBullet(UINT currentindex, OnSceneObject &on_scene_object);
    // 弾を表示するか管理する
    void ManagementBullets(std::vector<OnSceneObject> &on_scene_objects);
};
