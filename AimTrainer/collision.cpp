#include "collision.h"
using namespace std;
using namespace DirectX;

BoundingBox Collision::ComputeBoundingBox(vector<Mesh> &meshes) {
    vector<XMFLOAT3> vertices;

    // 頂点リストを作成
    for (uint8_t i = 0; i < meshes.size(); ++i) {
        const Mesh &mesh = meshes[i];

        // インデックスリスト作成
        for (uint16_t j = 0; j < mesh.indices.size(); ++j) {
            const uint32_t &index = mesh.indices[j];
            vertices.emplace_back(mesh.vertices[index].position);
        }
    }

    // 最も大きい座標と最も小さい座標を取得
    XMFLOAT3 min = {};
    XMFLOAT3 max = {};

    for (uint16_t i = 0; i < vertices.size(); ++i) {
        const XMFLOAT3 current_position = vertices[i];

        if (i == 0) {
            // 初回なら
            min = current_position;
            max = current_position;
        } else {
            // 各座標の大きさを比較
            if (min.x > current_position.x) {
                min.x = current_position.x;
            }
            if (min.y > current_position.y) {
                min.y = current_position.y;
            }
            if (min.z > current_position.z) {
                min.z = current_position.z;
            }

            if (max.x < current_position.x) {
                max.x = current_position.x;
            }
            if (max.y < current_position.y) {
                max.y = current_position.y;
            }
            if (max.z < current_position.z) {
                max.z = current_position.z;
            }
        }
    }

    return BoundingBox{min, max.x - min.x, max.y - min.y, max.z - min.z};
}

bool Collision::isCollisionPointToBoundingBox(DirectX::XMFLOAT3 &point_position, BoundingBox &boundingbox) {
    const XMFLOAT3 max = {boundingbox.min.x + boundingbox.size_x, boundingbox.min.y + boundingbox.size_y, boundingbox.min.z + boundingbox.size_z};
    if (point_position.x >= boundingbox.min.x && point_position.x <= max.x
        && point_position.y >= boundingbox.min.y && point_position.y <= max.y
        && point_position.z >= boundingbox.min.z && point_position.z <= max.z) {
        return true;
    }
    return false;
}
