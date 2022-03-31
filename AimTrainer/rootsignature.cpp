#include "rootsignature.h"
#include "engine.h"
#include <d3dx12.h>
using namespace std;

RootSignature::RootSignature() {
    auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // アプリケーションの入力アセンブラを使用する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS; // ドメインシェーダのルートシグネチャへのアクセスを拒否する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; // ハルシェーダのルートシグネチャへのアクセスを拒否する
    flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; // ジオメトリシェーダのルートシグネチャへのアクセスを拒否する

    CD3DX12_ROOT_PARAMETER rootparam[2] = {}; // 定数バッファとテクスチャの2
    rootparam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // b0の定数バッファを設定、全てのシェーダから見えるようにする

    CD3DX12_DESCRIPTOR_RANGE table_range[1] = {}; // ディスクリプタテーブル
    table_range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // シェーダーリソースビュー
    rootparam[1].InitAsDescriptorTable(size(table_range), table_range, D3D12_SHADER_VISIBILITY_ALL);

    // スタティックサンプラの設定
    auto sampler = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR);

    // ルートシグネチャの設定(設定したいルートパラメータとスタティックサンプラを入れる)
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = std::size(rootparam); // ルートパラメータの個数を入れる
    desc.NumStaticSamplers = 1; // サンプラの個数を入れる
    desc.pParameters = rootparam; // ルートパラメータのポインタを入れる
    desc.pStaticSamplers = &sampler; // サンプラのポインタを入れる
    desc.Flags = flag; // フラグを設定

    ComPtr<ID3DBlob> p_blob;
    ComPtr<ID3DBlob> p_errorblob;

    // シリアライズ
    auto hresult = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, p_blob.GetAddressOf(), p_errorblob.GetAddressOf());
    if (FAILED(hresult)) {
        printf("ルートシグネチャシリアライズに失敗");
        return;
    }

    // ルートシグネチャ生成
    hresult = gp_engine->Device()->CreateRootSignature(0, p_blob->GetBufferPointer(), p_blob->GetBufferSize(), IID_PPV_ARGS(mp_rootsignature.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        printf("ルートシグネチャの生成に失敗");
        return;
    }

    m_isvalid = true;
}

bool RootSignature::IsValid() {
    return m_isvalid;
}

ID3D12RootSignature *RootSignature::Get() {
    return mp_rootsignature.Get();
}
