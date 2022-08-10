#pragma once
#include "comptr.h"
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
