#pragma once
#include <cstdint>
#include <d3d12.h>
#include "comptr.h"

class IndexBuffer {
public:
    IndexBuffer(size_t size, const uint32_t *p_initdata = nullptr);
    bool IsValid();
    D3D12_INDEX_BUFFER_VIEW View() const;

private:
    bool m_isvalid = false;
    ComPtr<ID3D12Resource> mp_buffer; // �C���f�b�N�X�o�b�t�@
    D3D12_INDEX_BUFFER_VIEW m_view; // �C���f�b�N�X�o�b�t�@�r���[

    IndexBuffer(const IndexBuffer &) = delete;
    void operator=(const IndexBuffer &) = delete;
};