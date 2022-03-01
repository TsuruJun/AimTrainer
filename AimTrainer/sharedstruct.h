#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "comptr.h"

struct Vertex {
    DirectX::XMFLOAT3 position; // �ʒu���W
    DirectX::XMFLOAT3 normal; // �@��
    DirectX::XMFLOAT2 uv; // uv���W
    DirectX::XMFLOAT3 tangent; // �ڋ��
    DirectX::XMFLOAT4 color; // ���_�F
};

struct alignas(256) Transform {
    DirectX::XMMATRIX world; // ���[���h�s��
    DirectX::XMMATRIX view; // �r���[�s��
    DirectX::XMMATRIX proj; // ���e�s��
};