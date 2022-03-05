#pragma once
#include "comptr.h"
#include <d3dx12.h>
#include <string>

class PipelineState {
public:
    PipelineState(); // �R���X�g���N�^�ł�����x�̐ݒ������
    bool IsValid(); // �����ɐ�����������Ԃ�

    void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout); // ���̓��C�A�E�g��ݒ�
    void SetRootSignature(ID3D12RootSignature *rootsignature); // ���[�g�V�O�l�`����ݒ�
    void SetVertexShader(std::wstring filepath); // ���_�V�F�[�_��ݒ�
    void SetPixelShader(std::wstring filepath); // �s�N�Z���V�F�[�_��ݒ�
    void Create(); // �p�C�v���C���X�e�[�g�𐶐�

    ID3D12PipelineState *Get();

private:
    bool m_isvalid = false; // �����ɐ���������
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {}; // �p�C�v���C���X�e�[�g�̐ݒ�
    ComPtr<ID3D12PipelineState> mp_pipelinestate = nullptr; // �p�C�v���C���X�e�[�g
    ComPtr<ID3DBlob> mp_vertexshaderblob; // ���_�V�F�[�_
    ComPtr<ID3DBlob> mp_pixelshaderblob; // �s�N�Z���V�F�[�_
};