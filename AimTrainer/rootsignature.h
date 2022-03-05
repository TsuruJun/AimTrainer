#pragma once
#include "comptr.h"

struct ID3D12RootSignature;

class RootSignature {
public:
    RootSignature(); // コンストラクタでルートシグネチャを生成
    bool IsValid(); // ルートシグネチャの生成に成功したかを返す
    ID3D12RootSignature *Get(); // ルートシグネチャを返す

private:
    bool m_isvalid = false; // ルートシグネチャの生成に成功したか
    ComPtr<ID3D12RootSignature> mp_rootsignature = nullptr;
};