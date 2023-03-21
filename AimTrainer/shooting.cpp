#include "shooting.h"
#include "sharedstruct.h"
#include "descriptorheap.h"
#include "texture2D.h"
#include "app.h"
#include "engine.h"
#include <d3dx12.h>
#include <filesystem>
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

// 拡張子を置き換える処理
wstring TempReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}

bool Shooting::Shoot(
    vector<Mesh> &object,
    vector<vector<VertexBuffer *>> &vertex_buffers,
    vector<vector<IndexBuffer *>> &index_buffers,
    vector<vector<DescriptorHandle *>> &material_handles,
    vector<ConstantBuffer *> &constantbuffers,
    XMFLOAT3 targetposition) {
    // 頂点バッファを追加
    vertex_buffers.resize(vertex_buffers.size() + 1);
    for (size_t i = 0; i < object.size(); ++i) {
        auto size = sizeof(Vertex) * object[i].vertices.size();
        auto stride = sizeof(Vertex);
        auto vertices = object[i].vertices.data();
        auto p_vertex_buffer = new VertexBuffer(size, stride, vertices);
        if (!p_vertex_buffer->IsValid()) {
            printf("頂点バッファの生成に失敗\n");
            return false;
        }

        vertex_buffers[vertex_buffers.size() - 1].emplace_back(p_vertex_buffer);
    }

    // インデックスバッファ追加
    index_buffers.resize(index_buffers.size() + 1);
    for (size_t i = 0; i < object.size(); ++i) {
        auto size = sizeof(uint32_t) * object[i].indices.size();
        auto indices = object[i].indices.data();
        auto p_index_buffer = new IndexBuffer(size, indices);
        if (!p_index_buffer->IsValid()) {
            printf("インデックスバッファの生成に失敗");
            return false;
        }

        index_buffers[index_buffers.size() - 1].emplace_back(p_index_buffer);
    }

    // マテリアル読み込み
    material_handles.resize(material_handles.size() + 1);
    DescriptorHeap *p_descriptor_heap = new DescriptorHeap();
    for (size_t i = 0; i < object.size(); ++i) {
        auto texture_path = TempReplaceExtension(object[i].diffusemap, "tga");
        auto main_texture = Texture2D::Get(texture_path);
        auto handle = p_descriptor_heap->Register(main_texture);

        material_handles[material_handles.size() - 1].emplace_back(handle);
    }

    // カメラ設定
    const XMFLOAT3 eyeposition = {0.0f, 1.0f, 0.0f}; // 視点の位置
    const XMFLOAT3 upward = {0.0f, 1.0f, 0.0f}; // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(52.0f); // 視野角
    constexpr auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; ++i) {
        constantbuffers.emplace_back(new ConstantBuffer(sizeof(Transform)));
        if (!constantbuffers.back()->IsValid()) {
            printf("変換行列用定数バッファの生成に失敗");
            return false;
        }

        // 変換行列の登録
        auto ptr = constantbuffers[i]->GetPtr<Transform>();
        ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        ptr->view = XMMatrixLookAtLH(XMLoadFloat3(&eyeposition), XMLoadFloat3(&targetposition), XMLoadFloat3(&upward));
        ptr->proj = XMMatrixPerspectiveFovLH(fov, aspect, 0.3f, 1000.f);
    }

    // 管理用にbulletの構造体を作成
    Bullet bullet = {
        vertex_buffers[vertex_buffers.size() - 1],
        index_buffers[index_buffers.size() - 1],
        material_handles[material_handles.size() - 1],
        constantbuffers[constantbuffers.size() - 2],
        constantbuffers[constantbuffers.size() - 1],
        std::chrono::system_clock::now(),
        false,
        targetposition,
        vertex_buffers.size() - 1
    };

    // 管理用vectorに格納
    bullets.emplace_back(bullet);

    return true;
}

bool Shooting::MoveBullets(UINT currentindex, std::vector<ConstantBuffer *> &constantbuffers) {
    for (uint32_t i = 0; i < bullets.size(); ++i) {
        const Bullet bullet = bullets[i];
        const uint32_t index = bullet.index * 2;
        XMMATRIX &now_world = constantbuffers[currentindex + index]->GetPtr<Transform>()->world;

        constexpr float offset = 10.0f;
        const float translation_position_x = now_world.r[3].m128_f32[0] + (bullet.bullet_vector.x * offset);
        const float translation_position_y = now_world.r[3].m128_f32[1] + (bullet.bullet_vector.y * offset);
        const float translation_position_z = now_world.r[3].m128_f32[2] + (bullet.bullet_vector.z * offset);

        now_world = XMMatrixTranslation(translation_position_x, translation_position_y, translation_position_z);
    }
    return true;
}

bool Shooting::ManagementBullets(
    vector<vector<VertexBuffer *>> &vertex_buffers,
    vector<vector<IndexBuffer *>> &index_buffers,
    vector<vector<DescriptorHandle *>> &material_handles,
    vector<ConstantBuffer *> &constantbuffers) {
    // 表示する必要のない弾は削除
    // シーンの配列の要素も消えるので、空の要素を削除して詰める

    return true;
}
