#pragma once
#include "sharedstruct.h"
#include <vector>
#include <DirectXMath.h>

class Collision {
public:
    BoundingBox ComputeBoundingBox(std::vector<Mesh> &meshes);
    bool isCollisionPointToBoundingBox(DirectX::XMFLOAT3 &point_position, BoundingBox &boundingbox);
};
