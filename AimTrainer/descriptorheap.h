#pragma once
#include "comptr.h"
#include <d3dx12.h>
#include <vector>

class ConstantBuffer;
class Texture2D;

class DescriptorHandle {
public:
    D3D12_CPU_DESCRIPTOR_HANDLE m_handle_CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE m_handle_GPU;
};

class DescriptorHeap {
public:
    DescriptorHeap(); // �R���X�g���N�^�ō쐬����
    ID3D12DescriptorHeap *GetHeap(); // �f�B�X�N���v�^�q�[�v��Ԃ�
    DescriptorHandle *Register(Texture2D *texture); // �e�N�X�`�����f�B�X�N���v�^�q�[�v�ɓo�^���A�n���h����Ԃ�

private:
    bool m_isvalid = false; // �����ɐ����������ǂ���
    UINT m_increment_size = 0;
    ComPtr<ID3D12DescriptorHeap> mp_heap = nullptr; // �f�B�X�N���v�^�q�[�v�{��
    std::vector<DescriptorHandle *> mp_handles; // �o�^����Ă���n���h��
};