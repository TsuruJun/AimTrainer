#pragma once
#include "comptr.h"
#include <d3dx12.h>
#include <string>

class PipelineState {
public:
    PipelineState(); // コンストラクタである程度の設定をする
    bool IsValid(); // 生成に成功したかを返す

    void SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout); // 入力レイアウトを設定
    void SetRootSignature(ID3D12RootSignature *rootsignature); // ルートシグネチャを設定
    void SetVertexShader(std::wstring filepath); // 頂点シェーダを設定
    void SetPixelShader(std::wstring filepath); // ピクセルシェーダを設定
    void Create(); // パイプラインステートを生成

    ID3D12PipelineState *Get();

private:
    bool m_isvalid = false; // 生成に成功したか
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {}; // パイプラインステートの設定
    ComPtr<ID3D12PipelineState> mp_pipelinestate = nullptr; // パイプラインステート
    ComPtr<ID3DBlob> mp_vertexshaderblob; // 頂点シェーダ
    ComPtr<ID3DBlob> mp_pixelshaderblob; // ピクセルシェーダ
};