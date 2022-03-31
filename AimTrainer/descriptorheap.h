#pragma once
#include "comptr.h"
#include <d3dx12.h>
#include <vector>

class ConstantBuffer;
class Texture2D;

class DescriptorHandle {
public:
    D3D12_CPU_DESCRIPTOR_HANDLE m_handle_CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE m_handle_GPU;
};

class DescriptorHeap {
public:
    DescriptorHeap(); // コンストラクタで作成する
    ID3D12DescriptorHeap *GetHeap(); // ディスクリプタヒープを返す
    DescriptorHandle *Register(Texture2D *texture); // テクスチャをディスクリプタヒープに登録し、ハンドルを返す

private:
    bool m_isvalid = false; // 生成に成功したかどうか
    UINT m_increment_size = 0;
    ComPtr<ID3D12DescriptorHeap> mp_heap = nullptr; // ディスクリプタヒープ本体
    std::vector<DescriptorHandle *> mp_handles; // 登録されているハンドル
};