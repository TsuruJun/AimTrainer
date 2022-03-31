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
    static const D3D12_INPUT_LAYOUT_DESC inputlayout;

private:
    static const int inputelementcount = 5;
    static const D3D12_INPUT_ELEMENT_DESC inputelements[inputelementcount];
};

struct alignas(256) Transform {
    DirectX::XMMATRIX world; // ���[���h�s��
    DirectX::XMMATRIX view; // �r���[�s��
    DirectX::XMMATRIX proj; // ���e�s��
};

struct Mesh {
    std::vector<Vertex> vertices; // ���_�f�[�^�̔z��
    std::vector<uint32_t> indices; // �C���f�b�N�X�̔z��
    std::wstring diffusemap; // �e�N�X�`���̃t�@�C���p�X
};