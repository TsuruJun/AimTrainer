#pragma once
#include "comptr.h"
#include <d3dx12.h>

class ConstantBuffer {
public:
    ConstantBuffer(size_t size); // コンストラクタで定数バッファを生成
    bool IsValid(); // バッファ生成に成功したかを返す
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const; // バッファのGPU上のアドレスを返す
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc(); //定数バッファビューを返す

    void *GetPtr() const; // 定数バッファにマッピングされたポインタを返す

    template <typename T>
    T *GetPtr() {
        return reinterpret_cast<T *>(GetPtr());
    }

private:
    bool m_isvalid = false; // 定数バッファ生成に成功したか
    ComPtr<ID3D12Resource> mp_buffer = nullptr; // 定数バッファ
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc = {}; //定数バッファビューの設定
    void *mp_mappedptr = nullptr;

    ConstantBuffer(const ConstantBuffer &) = delete;
    void operator=(const ConstantBuffer &) = delete;
};
