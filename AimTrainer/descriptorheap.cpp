#include "descriptorheap.h"
#include "texture2D.h"
#include <d3dx12.h>
#include "engine.h"

const UINT HANDLE_MAX = 512;

DescriptorHeap::DescriptorHeap() {
    mp_handles.clear();
    mp_handles.reserve(HANDLE_MAX);

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = HANDLE_MAX;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto device = gp_engine->Device();

    // �f�B�X�N���v�^�q�[�v���쐬
    auto hresult = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mp_heap.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        m_isvalid = false;
        return;
    }

    m_increment_size = device->GetDescriptorHandleIncrementSize(desc.Type); // �f�B�X�N���v�^�q�[�v1�̃������T�C�Y��Ԃ�
    m_isvalid = true;
}

ID3D12DescriptorHeap *DescriptorHeap::GetHeap() {
    return mp_heap.Get();
}

DescriptorHandle *DescriptorHeap::Register(Texture2D *texture) {
    auto count = mp_handles.size();
    if (HANDLE_MAX <= count) {
        return nullptr;
    }

    DescriptorHandle *p_handle = new DescriptorHandle();

    auto handle_CPU = mp_heap->GetCPUDescriptorHandleForHeapStart(); // �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
    handle_CPU.ptr += m_increment_size * count; // �ŏ��̃A�h���X����count�Ԗڂ�����ǉ����ꂽ���\�[�X�̃n���h��

    auto handle_GPU = mp_heap->GetGPUDescriptorHandleForHeapStart(); // �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
    handle_GPU.ptr += m_increment_size * count; // �ŏ��̃A�h���X����count�Ԗڂ�����ǉ����ꂽ���\�[�X�̃n���h��

    p_handle->m_handle_CPU = handle_CPU;
    p_handle->m_handle_GPU = handle_GPU;

    auto resource = texture->Resource();
    auto desc = texture->ViewDesc();
    gp_engine->Device()->CreateShaderResourceView(resource, &desc, p_handle->m_handle_CPU); // �V�F�[�_�[���\�[�X�r���[�쐬

    mp_handles.emplace_back(p_handle);
    return p_handle;
}
