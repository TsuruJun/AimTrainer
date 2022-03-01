#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "comptr.h"

struct Vertex {
    DirectX::XMFLOAT3 position; // 位置座標
    DirectX::XMFLOAT3 normal; // 法線
    DirectX::XMFLOAT2 uv; // uv座標
    DirectX::XMFLOAT3 tangent; // 接空間
    DirectX::XMFLOAT4 color; // 頂点色
};

struct alignas(256) Transform {
    DirectX::XMMATRIX world; // ワールド行列
    DirectX::XMMATRIX view; // ビュー行列
    DirectX::XMMATRIX proj; // 投影行列
};