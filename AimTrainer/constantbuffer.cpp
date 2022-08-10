#include "constantbuffer.h"
#include "engine.h"

ConstantBuffer::ConstantBuffer(size_t size) {
    size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    UINT64 sizealigned = (size + (align - 1)) & ~(align - 1); // alignに切り上げる
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizealigned); // リソースの設定

    // リソースを生成
    auto hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(mp_buffer.GetAddressOf()));
    if (FAILED(hresult)) {
        printf("定数バッファリソースの生成に失敗");
        return;
    }

    hresult = mp_buffer->Map(0, nullptr, &mp_mappedptr);
    if (FAILED(hresult)) {
        printf("定数バッファのマッピングに失敗");
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
