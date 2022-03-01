#include "constantbuffer.h"
#include "engine.h"

ConstantBuffer::ConstantBuffer(size_t size) {
    size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    UINT64 sizealigned = (size + (align - 1)) & ~(align - 1); // align�ɐ؂�グ��
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // �q�[�v�v���p�e�B
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizealigned); // ���\�[�X�̐ݒ�

    // ���\�[�X�𐶐�
    auto hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mp_buffer.GetAddressOf()));
    if (FAILED(hresult)) {
        printf("�萔�o�b�t�@���\�[�X�̐����Ɏ��s");
        return;
    }

    hresult = mp_buffer->Map(0, nullptr, &mp_mappedptr);
    if (FAILED(hresult)) {
        printf("�萔�o�b�t�@�̃}�b�s���O�Ɏ��s");
        return;
    }

    m_desc = {};
    m_desc.BufferLocation = mp_buffer->GetGPUVirtualAddress();
    m_desc.SizeInBytes = UINT(sizealigned);

    m_isvalid = true;
}

bool ConstantBuffer::IsValid() {
    return m_isvalid;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const {
    return m_desc.BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc() {
    return m_desc;
}

void *ConstantBuffer::GetPtr() const {
    return mp_mappedptr;
}
