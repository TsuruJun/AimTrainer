#include "engine.h"
#include <stdio.h>
#include <Windows.h>
#include <d3dx12.h>

Engine *gp_engine;

bool Engine::Init(HWND hwnd, UINT window_width, UINT window_height) {
    m_framebuffer_width = window_width;
    m_framebuffer_height = window_height;
    m_hwnd = hwnd;

    if (!CreateDevice()) {
        printf("デバイスの生成に失敗");
        return false;
    }

    if (!CreateCommandQueue()) {
        printf("コマンドキューの生成に失敗");
        return false;
    }

    if (!CreateSwapChain()) {
        printf("スワップチェインの生成に失敗");
        return false;
    }

    if (!CreateCommandList()) {
        printf("コマンドリストの生成に失敗");
        return false;
    }

    if (!CreateFence()) {
        printf("フェンスの生成に失敗");
        return false;
    }

    // ビューポートとシザー短形を生成
    CreateViewPort();
    CreateScissorRect();

    if (!CreateRenderTarget()) {
        printf("レンダーターゲットの生成に失敗");
        return false;
    }

    if (!CreateDepthStencil()) {
        printf("デプスステンシルバッファの生成に失敗");
        return false;
    }

    printf("描画エンジンの初期化に成功\n");
    return true;
}

void Engine::BeginRender() {
    // 現在のレンダーゲットを更新
    mp_currentrendertarget = mp_renderTargets[m_currentbackbufferindex].Get();

    // コマンドを初期化してためる準備をする
    mp_allocator[m_currentbackbufferindex]->Reset();
    mp_commandlist->Reset(mp_allocator[m_currentbackbufferindex].Get(), nullptr);

    // ビューポートとシザー短形を設定
    mp_commandlist->RSSetViewports(1, &m_viewport);
    mp_commandlist->RSSetScissorRects(1, &m_scissor);

    // 現在のフレームのレンダーターゲットビューのディスクリプタヒープの開始アドレスを取得
    auto currentrtvhandle = mp_rtvheap->GetCPUDescriptorHandleForHeapStart();
    currentrtvhandle.ptr += m_currentbackbufferindex * m_rtvdescriptorsize;

    // 深度ステンシルのディスクリプタヒープの開始アドレス取得
    auto currentdsvhandle = mp_dsvheap->GetCPUDescriptorHandleForHeapStart();

    // レンダーターゲットが使用可能になるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mp_currentrendertarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mp_commandlist->ResourceBarrier(1, &barrier);

    // レンダーターゲットを設定
    mp_commandlist->OMSetRenderTargets(1, &currentrtvhandle, FALSE, &currentdsvhandle);

    // レンダーターゲットをクリア
    const float clearcolor[] = {0.25f, 0.25f, 0.25f, 1.0f};
    mp_commandlist->ClearRenderTargetView(currentrtvhandle, clearcolor, 0, nullptr);

    // 深度ステンシルビューをクリア
    mp_commandlist->ClearDepthStencilView(currentdsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

}

void Engine::EndRender() {
    // レンダーターゲットに書き込み終わるまで待つ
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mp_currentrendertarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mp_commandlist->ResourceBarrier(1, &barrier);

    // コマンドの記録を終了
    mp_commandlist->Close();

    // コマンドを実行
    ID3D12CommandList *p_commandlists[] = {mp_commandlist.Get()};
    mp_queue->ExecuteCommandLists(1, p_commandlists);

    // スワップチェーンを切り替え
    mp_swapchain->Present(1, 0);

    // 描画完了を待つ
    WaitRender();

    // バックバッファ番号を更新
    m_currentbackbufferindex = mp_swapchain->GetCurrentBackBufferIndex();
}

ID3D12Device6 *Engine::Device() {
    return mp_device.Get();
}

ID3D12GraphicsCommandList *Engine::CommandList() {
    return mp_commandlist.Get();
}

UINT Engine::CurrentBackBufferIndex() {
    return m_currentbackbufferindex;
}

bool Engine::CreateDevice() {
    auto hresult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(mp_device.ReleaseAndGetAddressOf()));
    return SUCCEEDED(hresult);
}

bool Engine::CreateCommandQueue() {
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    auto hresult = mp_device->CreateCommandQueue(&desc, IID_PPV_ARGS(mp_queue.ReleaseAndGetAddressOf()));

    return SUCCEEDED(hresult);
}

bool Engine::CreateSwapChain() {
    //DXGIファクトリーの生成
    IDXGIFactory4 *p_factory = nullptr;
    HRESULT hresult = CreateDXGIFactory1(IID_PPV_ARGS(&p_factory));
    if (FAILED(hresult)) {
        return false;
    }

    // スワップチェインの生成
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = m_framebuffer_width;
    desc.BufferDesc.Height = m_framebuffer_height;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = FRAME_BUFFER_COUNT;
    desc.OutputWindow = m_hwnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain *p_swapchain = nullptr;
    hresult = p_factory->CreateSwapChain(mp_queue.Get(), &desc, &p_swapchain);
    if (FAILED(hresult)) {
        p_factory->Release();
        return false;
    }

    // IDXGISwapChain3を取得
    hresult = p_swapchain->QueryInterface(IID_PPV_ARGS(mp_swapchain.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        p_factory->Release();
        p_swapchain->Release();
        return false;
    }

    // バックバッファ番号を取得
    m_currentbackbufferindex = mp_swapchain->GetCurrentBackBufferIndex();

    p_factory->Release();
    p_swapchain->Release();

    return true;
}

bool Engine::CreateCommandList() {
    // コマンドアロケータの作成
    HRESULT hresult = 0;
    for (size_t i = 0; i < FRAME_BUFFER_COUNT; ++i) {
        hresult = mp_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mp_allocator[i].ReleaseAndGetAddressOf()));
    }

    if (FAILED(hresult)) {
        return false;
    }

    // コマンドリストの生成
    hresult = mp_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mp_allocator[m_currentbackbufferindex].Get(), nullptr, IID_PPV_ARGS(&mp_commandlist));
    if (FAILED(hresult)) {
        return false;
    }

    // コマンドリストは開かれている状態で作成されるので、一旦閉じる
    mp_commandlist->Close();

    return true;
}

bool Engine::CreateFence() {
    for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i) {
        m_fencevalue[i] = 0;
    }

    auto hresult = mp_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mp_fence.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        return false;
    }

    ++m_fencevalue[m_currentbackbufferindex];

    // 同期を行うときのイベントハンドラを生成する
    m_fenceevent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return m_fenceevent != nullptr;
}

void Engine::CreateViewPort() {
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.Width = static_cast<float>(m_framebuffer_width);
    m_viewport.Height = static_cast<float>(m_framebuffer_height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0;
}

void Engine::CreateScissorRect() {
    m_scissor.left = 0;
    m_scissor.right = m_framebuffer_width;
    m_scissor.top = 0;
    m_scissor.bottom = m_framebuffer_height;
}

bool Engine::CreateRenderTarget() {
    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = FRAME_BUFFER_COUNT;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hresult = mp_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mp_rtvheap.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        return false;
    }

    // ディスクリプタのサイズを取得
    m_rtvdescriptorsize = mp_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = mp_rtvheap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i) {
        mp_swapchain->GetBuffer(i, IID_PPV_ARGS(mp_renderTargets[i].ReleaseAndGetAddressOf()));
        mp_device->CreateRenderTargetView(mp_renderTargets[i].Get(), nullptr, rtvhandle);
        rtvhandle.ptr += m_rtvdescriptorsize;
    }

    return true;
}

bool Engine::CreateDepthStencil() {
    // DSV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC heapdesc = {};
    heapdesc.NumDescriptors = 1;
    heapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hresult = mp_device->CreateDescriptorHeap(&heapdesc, IID_PPV_ARGS(&mp_dsvheap));
    if (FAILED(hresult)) {
        return false;
    }

    // ディスクリプタのサイズを取得
    m_dsvdescriptorsize = mp_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    D3D12_CLEAR_VALUE dsvclearvalue = {};
    dsvclearvalue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvclearvalue.DepthStencil.Depth = 1.0f;
    dsvclearvalue.DepthStencil.Stencil = 0;

    auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourcedesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        m_framebuffer_width,
        m_framebuffer_height,
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

    hresult = mp_device->CreateCommittedResource(&heapprop, D3D12_HEAP_FLAG_NONE, &resourcedesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsvclearvalue, IID_PPV_ARGS(mp_depthstencilbuffer.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        return false;
    }

    // ディスクリプタを生成
    D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle = mp_dsvheap->GetCPUDescriptorHandleForHeapStart();

    mp_device->CreateDepthStencilView(mp_depthstencilbuffer.Get(), nullptr, dsvhandle);

    return true;
}

void Engine::WaitRender() {
    // 描画終了待ち
    const UINT64 fencevalue = m_fencevalue[m_currentbackbufferindex];
    mp_queue->Signal(mp_fence.Get(), fencevalue);
    ++m_fencevalue[m_currentbackbufferindex];

    // 次のフレームの描画準備がまだであれば待機する
    if (mp_fence->GetCompletedValue() < fencevalue) {
        // 完了時にイベントを設定
        auto hresult = mp_fence->SetEventOnCompletion(fencevalue, m_fenceevent);
        if (FAILED(hresult)) {
            return;
        }

        // 待機処理
        if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_fenceevent, INFINITE, FALSE)) {
            return;
        }
    }
}
