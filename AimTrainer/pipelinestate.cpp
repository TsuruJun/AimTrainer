#include "pipelinestate.h"
#include "engine.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState() {
    // �p�C�v���C���X�e�[�g�̐ݒ�
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ���X�^���C�U�̓f�t�H���g
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�͂Ȃ�
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // �u�����h�X�e�[�g���f�t�H���g
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // �[�x�X�e���V���̓f�t�H���g���g��
    desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // �O�p�`��`��
    desc.NumRenderTargets = 1; // �`��Ώۂ�1
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1; // �T���v����1
    desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid() {
    return m_isvalid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout) {
    desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature *rootsignature) {
    desc.pRootSignature = rootsignature;
}

void PipelineState::SetVertexShader(std::wstring filepath) {
    // ���_�V�F�[�_�ǂݍ���
    auto hresult = D3DReadFileToBlob(filepath.c_str(), mp_vertexshaderblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("���_�V�F�[�_�̓ǂݍ��݂Ɏ��s\n");
        return;
    }

    desc.VS = CD3DX12_SHADER_BYTECODE(mp_vertexshaderblob.Get());
}

void PipelineState::SetPixelShader(std::wstring filepath) {
    auto hresult = D3DReadFileToBlob(filepath.c_str(), mp_pixelshaderblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("�s�N�Z���V�F�[�_�̓ǂݍ��݂Ɏ��s\n");
        return;
    }

    desc.PS = CD3DX12_SHADER_BYTECODE(mp_pixelshaderblob.Get());
}

void PipelineState::Create() {
    // �p�C�v���C���X�e�[�g�𐶐�
    auto hresult = gp_engine->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(mp_pipelinestate.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
        return;
    }

    m_isvalid = true;
}

ID3D12PipelineState *PipelineState::Get() {
    return mp_pipelinestate.Get();
}
