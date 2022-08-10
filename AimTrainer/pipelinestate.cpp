#include "pipelinestate.h"
#include "engine.h"
#include <d3dcompiler.h>
#include <d3dx12.h>

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState() {
    // パイプラインステートの設定
    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ラスタライザはデフォルト
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングはなし
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // ブレンドステートもデフォルト
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // 深度ステンシルはデフォルトを使う
    desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形を描画
    desc.NumRenderTargets = 1; // 描画対象は1
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1; // サンプラは1
    desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid() {
    return m_isvalid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout) {
    desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature *rootsignature) {
    desc.pRootSignature = rootsignature;
}

void PipelineState::SetVertexShader(std::wstring filepath) {
    // 頂点シェーダ読み込み
    auto hresult = D3DReadFileToBlob(filepath.c_str(), mp_vertexshaderblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("頂点シェーダの読み込みに失敗\n");
        return;
    }

    desc.VS = CD3DX12_SHADER_BYTECODE(mp_vertexshaderblob.Get());
}

void PipelineState::SetPixelShader(std::wstring filepath) {
    auto hresult = D3DReadFileToBlob(filepath.c_str(), mp_pixelshaderblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("ピクセルシェーダの読み込みに失敗\n");
        return;
    }

    desc.PS = CD3DX12_SHADER_BYTECODE(mp_pixelshaderblob.Get());
}

void PipelineState::Create() {
    // パイプラインステートを生成
    auto hresult = gp_engine->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(mp_pipelinestate.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        printf("パイプラインステートの生成に失敗\n");
        return;
    }

    m_isvalid = true;
}

ID3D12PipelineState *PipelineState::Get() {
    return mp_pipelinestate.Get();
}
