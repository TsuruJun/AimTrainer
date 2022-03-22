#include "indexbuffer.h"
#include <d3dx12.h>
#include "engine.h"

IndexBuffer::IndexBuffer(size_t size, const uint32_t *p_initdata) {
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // �q�[�v�v���p�e�B
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size); // ���\�[�X�̐ݒ�

    // ���\�[�X�𐶐�
    auto hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mp_buffer.GetAddressOf()));
    if (FAILED(hresult)) {
        printf("[OnInit] �C���f�b�N�X�o�b�t�@���\�[�X�̐����Ɏ��s");
        return;
    }

    // �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�
    m_view = {};
    m_view.BufferLocation = mp_buffer->GetGPUVirtualAddress();
    m_view.Format = DXGI_FORMAT_R32_UINT;
    m_view.SizeInBytes = static_cast<UINT>(size);

    // �}�b�s���O����
    if (p_initdata != nullptr) {
        void *ptr = nullptr;
        hresult = mp_buffer->Map(0, nullptr, &ptr);
        if (FAILED(hresult)) {
            printf("[OnInit] �C���f�b�N�X�o�b�t�@�}�b�s���O�Ɏ��s");
            return;
        }

        // �C���f�b�N�X�f�[�^���}�b�s���O��ɐݒ�
        memcpy(ptr, p_initdata, size);

        // �}�b�s���O����
        mp_buffer->Unmap(0, nullptr);
    }
    m_isvalid = true;
}

bool IndexBuffer::IsValid() {
    return m_isvalid;
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::View() const {
    return m_view;
}
