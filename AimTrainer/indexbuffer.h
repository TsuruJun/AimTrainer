#pragma once
#include "comptr.h"
#include <cstdint>
#include <d3d12.h>

class IndexBuffer {
public:
    IndexBuffer(size_t size, const uint32_t *p_initdata = nullptr);
    bool IsValid();
    D3D12_INDEX_BUFFER_VIEW View() const;

private:
    bool m_isvalid = false;
    ComPtr<ID3D12Resource> mp_buffer; // インデックスバッファ
    D3D12_INDEX_BUFFER_VIEW m_view; // インデックスバッファビュー

    IndexBuffer(const IndexBuffer &) = delete;
    void operator=(const IndexBuffer &) = delete;
};
