#pragma once
#include "comptr.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "descriptorheap.h"
#include "constantbuffer.h"
#include "engine.h"
#include <chrono>
#include <DirectXMath.h>
#include <d3dx12.h>

struct Vertex {
    DirectX::XMFLOAT3 position; // 位置座標
    DirectX::XMFLOAT3 normal; // 法線
    DirectX::XMFLOAT2 uv; // uv座標
    DirectX::XMFLOAT3 tangent; // 接空間
    DirectX::XMFLOAT4 color; // 頂点色
    static const D3D12_INPUT_LAYOUT_DESC inputlayout;

private:
    static const int inputelementcount = 5;
    static const D3D12_INPUT_ELEMENT_DESC inputelements[inputelementcount];
};

struct alignas(256) Transform {
    DirectX::XMMATRIX world; // ワールド行列
    DirectX::XMMATRIX view; // ビュー行列
    DirectX::XMMATRIX proj; // 投影行列
};

struct Mesh {
    std::vector<Vertex> vertices; // 頂点データの配列
    std::vector<uint32_t> indices; // インデックスの配列
    std::wstring diffusemap; // テクスチャのファイルパス
};

struct BoundingBox {
    DirectX::XMFLOAT3 min;
    float size_x;
    float size_y;
    float size_z;
};

// シーン上に配置されるオブジェクトを管理する構造体を作成
struct OnSceneObject {
    std::vector<Mesh *> object;
    std::vector<VertexBuffer *> vertex_buffer;
    std::vector<IndexBuffer *> index_buffer;
    std::vector<DescriptorHandle *> material_handle;
    std::vector<ConstantBuffer *> constantbuffers;
    BoundingBox boundingbox;
    // オブジェクトが弾か
    bool is_bullet = false;
    // 弾が撃たれた時間
    long long shooted_datetime;
    // 弾が標的に当たったか
    bool hit = false;
    // 弾が飛んでいく方向
    DirectX::XMFLOAT3 bullet_vector;
};
