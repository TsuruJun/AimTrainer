#pragma once
#include "comptr.h"
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Engine {
public:
    enum {
        FRAME_BUFFER_COUNT = 2 // ダブルバッファリングするので2
    };

public:
    bool Init(HWND hwnd, UINT window_width, UINT window_height); // エンジン初期化

    void BeginRender(); // 描画の開始処理
    void EndRender(); // 描画の終了処理

public:
    // 外からアクセスしたいのでGetterとして公開するもの
    ID3D12Device6 *Device();
    ID3D12GraphicsCommandList *CommandList();
    UINT CurrentBackBufferIndex();
    HWND GetHwnd();

private:
    // DirextX12初期化に使う関数たち
    bool CreateDevice(); // デバイスを生成
    bool CreateCommandQueue(); // コマンドキューを生成
    bool CreateSwapChain(); // スワップチェインを生成
    bool CreateCommandList(); // コマンドリストとコマンドアロケータを生成
    bool CreateFence(); // フェンスを生成
    void CreateViewPort(); // ビューポートを生成
    void CreateScissorRect(); //シザー短形を生成

private:
    // 描画に使うDirectX12のオブジェクトたち
    HWND m_hwnd;
    UINT m_framebuffer_width = 0;
    UINT m_framebuffer_height = 0;
    UINT m_currentbackbufferindex = 0;

    ComPtr<ID3D12Device6> mp_device = nullptr; // デバイス
    ComPtr<ID3D12CommandQueue> mp_queue = nullptr; // コマンドキュー
    ComPtr<IDXGISwapChain3> mp_swapchain = nullptr; // スワップチェイン
    ComPtr<ID3D12CommandAllocator> mp_allocator[FRAME_BUFFER_COUNT] = {nullptr}; // コマンドアロケータ
    ComPtr<ID3D12GraphicsCommandList> mp_commandlist = nullptr; // コマンドリスト
    HANDLE m_fenceevent = nullptr; // フェンスで使うイベント
    ComPtr<ID3D12Fence> mp_fence = nullptr; // フェンス
    UINT64 m_fencevalue[FRAME_BUFFER_COUNT]; // フェンスの値(ダブルバッファリング用に2個)
    D3D12_VIEWPORT m_viewport; // ビューポート
    D3D12_RECT m_scissor; // シザー短形

private:
    // 描画に使うオブジェクトとその生成関数たち
    bool CreateRenderTarget(); // レンダーターゲットを生成
    bool CreateDepthStencil(); // 深度ステンシルバッファを生成

    UINT m_rtvdescriptorsize = 0; // レンダーターゲットビューのディスクリプタサイズ
    ComPtr<ID3D12DescriptorHeap> mp_rtvheap = nullptr; // レンダーターゲットのディスクリプタヒープ
    ComPtr<ID3D12Resource> mp_renderTargets[FRAME_BUFFER_COUNT] = {nullptr}; // レンダーターゲット(ダブルバッファリングするので2個)

    UINT m_dsvdescriptorsize = 0; // 深度ステンシルのディスクリプタサイズ
    ComPtr<ID3D12DescriptorHeap> mp_dsvheap = nullptr; // 深度ステンシルのディスクリプタヒープ
    ComPtr<ID3D12Resource> mp_depthstencilbuffer = nullptr; // 深度ステンシルバッファ

private:
    // 描画ループで使用するもの
    ID3D12Resource *mp_currentrendertarget = nullptr; // 現在のフレームのレンダーターゲットを一時的に保存
    void WaitRender(); // 描画完了を待つ処理
};

extern Engine *gp_engine; // どこからでも参照したいのでグローバルにする
