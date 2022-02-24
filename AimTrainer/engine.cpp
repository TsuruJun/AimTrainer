#include "engine.h"
#include <stdio.h>
#include <Windows.h>

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

    printf("描画エンジンの初期化に成功\n");
    return true;
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
    m_currentbackbuffer_index = mp_swapchain->GetCurrentBackBufferIndex();

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
    hresult = mp_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mp_allocator[m_currentbackbuffer_index].Get(), nullptr, IID_PPV_ARGS(&mp_commandlist));
    if (FAILED(hresult)) {
        return false;
    }

    // コマンドリストは開かれている状態で作成されるので、一旦閉じる
    mp_commandlist->Close();

    return true;
}
