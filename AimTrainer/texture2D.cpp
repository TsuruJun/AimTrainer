#include "texture2D.h"
#include "engine.h"
#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;
using namespace std;

// string(マルチバイト文字列)からwstring(ワイド文字列)を得る
wstring GetWideString(const string &str) {
    auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

    wstring wstr;
    wstr.resize(num1);

    auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

    assert(num1 == num2);
    return wstr;
}

// 拡張子を返す
wstring FileExtension(const wstring &path) {
    auto idx = path.rfind(L'.');
    return path.substr(idx + 1, path.length() - idx - 1);
}

Texture2D *Texture2D::Get(std::string path) {
    auto wpath = GetWideString(path);
    return Get(wpath);
}

Texture2D *Texture2D::Get(std::wstring path) {
    auto texture = new Texture2D(path);
    if (!texture->IsValid()) {
        return GetWhite(); // 読み込みに失敗したときは白単色テクスチャを返す
    }
    return texture;
}

Texture2D *Texture2D::GetWhite() {
    ID3D12Resource *buffer = GetDefaultResource(4, 4);

    vector<unsigned char> data(4 * 4 * 4);
    fill(data.begin(), data.end(), 0xff);

    auto hresult = buffer->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
    if (FAILED(hresult)) {
        return nullptr;
    }

    return new Texture2D(buffer);
}

bool Texture2D::IsValid() {
    return m_isvalid;
}

ID3D12Resource *Texture2D::Resource() {
    return mp_resource.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc() {
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = mp_resource->GetDesc().Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
    desc.Texture2D.MipLevels = 1;

    return desc;
}

Texture2D::Texture2D(string path) {
    m_isvalid = Load(path);
}

Texture2D::Texture2D(wstring path) {
    m_isvalid = Load(path);
}

Texture2D::Texture2D(ID3D12Resource *buffer) {
    mp_resource = buffer;
    m_isvalid = mp_resource != nullptr;
}

bool Texture2D::Load(std::string &path) {
    auto wpath = GetWideString(path);
    return Load(wpath);
}

bool Texture2D::Load(std::wstring &path) {
    // WICテクスチャのロード
    TexMetadata meta = {};
    ScratchImage scratch = {};
    auto extention = FileExtension(path);

    HRESULT hresult = S_FALSE;

    // pngの時はWICFileを使う
    if (extention == L"png") {
        LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
    } else if (extention == L"tga") { // tgaの時はTGAFileを使う
        hresult = LoadFromTGAFile(path.c_str(), &meta, scratch);
    }
    if (FAILED(hresult)) {
        return false;
    }

    auto image = scratch.GetImage(0, 0, 0);
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format, meta.width, meta.height, static_cast<UINT16>(meta.arraySize), static_cast<UINT16>(meta.mipLevels));

    // リソースを生成
    hresult = gp_engine->Device()->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(mp_resource.ReleaseAndGetAddressOf()));
    if (FAILED(hresult)) {
        return false;
    }

    return true;
}

ID3D12Resource *Texture2D::GetDefaultResource(size_t width, size_t height) {
    auto res_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
    auto texture_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
    ID3D12Resource *buffer = nullptr;

    auto hresult = gp_engine->Device()->CreateCommittedResource(&texture_heap_prop, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&buffer));
    if (FAILED(hresult)) {
        assert(SUCCEEDED(hresult));
        return nullptr;
    }

    return buffer;
}
