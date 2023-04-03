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
    bool Shoot(std::vector<Mesh> &object, std::vector<OnSceneObject> &on_scene_objects, float init_x, float init_y, float init_z, DirectX::XMFLOAT3 targetposition);
    // 弾を動かす
    bool MoveBullets(UINT currentindex, OnSceneObject &on_scene_object);
    // 弾を表示するか管理する
    bool ManagementBullets(OnSceneObject &on_scene_object);
};
