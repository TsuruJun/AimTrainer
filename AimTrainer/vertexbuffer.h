#pragma once
#include "comptr.h"
#include <d3d12.h>

class VertexBuffer {
public:
    VertexBuffer(size_t size, size_t stride, const void *p_initdata); // コンストラクタでバッファを作成
    D3D12_VERTEX_BUFFER_VIEW View() const; // 頂点バッファビューを取得
    bool IsValid(); // バッファの生成に成功したかを取得

private:
    bool m_isvalid = false; // バッファの生成に成功したかを取得
    ComPtr<ID3D12Resource> mp_buffer = nullptr; // バッファ
    D3D12_VERTEX_BUFFER_VIEW m_view = {}; // 頂点バッファビュー

    VertexBuffer(const VertexBuffer &) = delete;
    void operator=(const VertexBuffer &) = delete;
};
