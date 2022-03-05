#include "rootsignature.h"
#include "engine.h"
#include <d3dx12.h>

RootSignature::RootSignature() {
    auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // �A�v���P�[�V�����̓��̓A�Z���u�����g�p����
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS; // �h���C���V�F�[�_�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; // �n���V�F�[�_�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; // �W�I���g���V�F�[�_�̃��[�g�V�O�l�`���ւ̃A�N�Z�X�����ۂ���

    CD3DX12_ROOT_PARAMETER rootparam[1] = {};
    rootparam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // b0�̒萔�o�b�t�@��ݒ�A�S�ẴV�F�[�_���猩����悤�ɂ���

    // �X�^�e�B�b�N�T���v���̐ݒ�
    auto sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR);

    // ���[�g�V�O�l�`���̐ݒ�(�ݒ肵�������[�g�p�����[�^�ƃX�^�e�B�b�N�T���v��������)
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = std::size(rootparam); // ���[�g�p�����[�^�̌�������
    desc.NumStaticSamplers = 1; // �T���v���̌�������
    desc.pParameters = rootparam; // ���[�g�p�����[�^�̃|�C���^������
    desc.pStaticSamplers = &sampler; // �T���v���̃|�C���^������
    desc.Flags = flag; // �t���O��ݒ�

    ComPtr<ID3DBlob> p_blob;
    ComPtr<ID3DBlob> p_errorblob;

    // �V���A���C�Y
    auto hresult = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, p_blob.GetAddressOf(), p_errorblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("���[�g�V�O�l�`���V���A���C�Y�Ɏ��s");
        return;
    }

    // ���[�g�V�O�l�`������
    hresult = gp_engine->Device()->CreateRootSignature(0, p_blob->GetBufferPointer(), p_blob->GetBufferSize(), IID_PPV_ARGS(mp_rootsignature.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        printf("���[�g�V�O�l�`���̐����Ɏ��s");
        return;
    }

    m_isvalid = true;
}

bool RootSignature::IsValid() {
    return m_isvalid;
}

ID3D12RootSignature *RootSignature::Get() {
    return mp_rootsignature.Get();
}
