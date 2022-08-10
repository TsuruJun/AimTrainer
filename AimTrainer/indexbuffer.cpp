#include "indexbuffer.h"
#include "engine.h"
#include <d3dx12.h>

IndexBuffer::IndexBuffer(size_t size, const uint32_t *p_initdata) {
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // リソースを生成
    auto hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mp_buffer.GetAddressOf()));
    if (FAILED(hresult)) {
        printf("[OnInit] インデックスバッファリソースの生成に失敗");
        return;
    }

    // インデックスバッファビューの設定
    m_view = {};
    m_view.BufferLocation = mp_buffer->GetGPUVirtualAddress();
    m_view.Format = DXGI_FORMAT_R32_UINT;
    m_view.SizeInBytes = static_cast<UINT>(size);

    // マッピングする
    if (p_initdata != nullptr) {
        void *ptr = nullptr;
        hresult = mp_buffer->Map(0, nullptr, &ptr);
        if (FAILED(hresult)) {
            printf("[OnInit] インデックスバッファマッピングに失敗");
            return;
        }

        // インデックスデータをマッピング先に設定
        memcpy(ptr, p_initdata, size);

        // マッピング解除
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
