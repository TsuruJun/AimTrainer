#pragma once
#include "comptr.h"

struct ID3D12RootSignature;

class RootSignature {
public:
    RootSignature(); // �R���X�g���N�^�Ń��[�g�V�O�l�`���𐶐�
    bool IsValid(); // ���[�g�V�O�l�`���̐����ɐ�����������Ԃ�
    ID3D12RootSignature *Get(); // ���[�g�V�O�l�`����Ԃ�

private:
    bool m_isvalid = false; // ���[�g�V�O�l�`���̐����ɐ���������
    ComPtr<ID3D12RootSignature> mp_rootsignature = nullptr;
};