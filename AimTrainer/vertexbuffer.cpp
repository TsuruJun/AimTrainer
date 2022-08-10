#include "vertexbuffer.h"
#include "engine.h"
#include <d3dx12.h>

VertexBuffer::VertexBuffer(size_t size, size_t stride, const void *p_initdata) {
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // リソースを生成
    auto hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mp_buffer.GetAddressOf()));
    if (FAILED(hresult)) {
        printf("頂点バッファリソースの生成に失敗");
        return;
    }

    // 頂点バッファビューの設定
    m_view.BufferLocation = mp_buffer->GetGPUVirtualAddress();
    m_view.SizeInBytes = static_cast<UINT>(size);
    m_view.StrideInBytes = static_cast<UINT>(stride);

    // マッピングする
    if (p_initdata != nullptr) {
        void *ptr = nullptr;
        hresult = mp_buffer->Map(0, nullptr, &ptr);
        if (FAILED(hresult)) {
            printf("頂点マッピングに失敗");
            return;
        }

        // 頂点データをマッピング先に設定
        memcpy(ptr, p_initdata, size);

        // マッピング解除
        mp_buffer->Unmap(0, nullptr);
    }

    m_isvalid = true;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::View() const {
    return m_view;
}

bool VertexBuffer::IsValid() {
    return m_isvalid;
}
