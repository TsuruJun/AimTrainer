#pragma once
#include "sharedstruct.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "descriptorheap.h"
#include "constantbuffer.h"
#include <vector>
#include <chrono>
#include <DirectXMath.h>

class Shooting {
public:
    // 弾を撃つ
    bool Shoot(
        std::vector<Mesh> &object,
        std::vector<std::vector<VertexBuffer *>> &vertex_buffers,
        std::vector<std::vector<IndexBuffer *>> &index_buffers,
        std::vector<std::vector<DescriptorHandle *>> &material_handles,
        std::vector<ConstantBuffer *> &constantbuffers,
        DirectX::XMFLOAT3 targetposition
    );
    // 弾を動かす
    bool MoveBullets(UINT currentindex, std::vector<ConstantBuffer *> &constantbuffers);
    // 弾を表示するか管理する
    bool ManagementBullets(
        std::vector<std::vector<VertexBuffer *>> &vertex_buffers,
        std::vector<std::vector<IndexBuffer *>> &index_buffers,
        std::vector<std::vector<DescriptorHandle *>> &material_handles,
        std::vector<ConstantBuffer *> &constantbuffers
    );

private:
    struct Bullet {
        // バッファ関係を直接管理
        std::vector<VertexBuffer *> management_vertex_buffer;
        std::vector<IndexBuffer *> management_index_buffer;
        std::vector<DescriptorHandle *> management_material_handle;
        ConstantBuffer *management_constantbuffer_1;
        ConstantBuffer *management_constantbuffer_2;

        // 弾が撃たれた時間
        std::chrono::system_clock::time_point shooted_datetime;

        // 弾が標的に当たったか
        bool hit = false;

        // 弾が飛んでいく方向
        DirectX::XMFLOAT3 bullet_vector;

        // 弾が配列上のどこに位置するか
        uint32_t index;
    };

    std::vector<Bullet> bullets;
};
