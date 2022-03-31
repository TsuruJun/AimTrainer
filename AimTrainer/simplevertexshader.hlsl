cbuffer Transform : register(b0) {
    float4x4 world; // ワールド行列
    float4x4 view; // ビュー行列
    float4x4 proj; // 投影行列
}

struct VSInput {
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // uv
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR; // 頂点色
};

struct VSOutput {
    float4 svpos : SV_POSITION; // 変換された座標
    float4 color : COLOR; // 変換された色
    float2 uv : TEXCOORD;
};

VSOutput vert(VSInput input) {
    VSOutput output = (VSOutput)0; // アウトプット構造体を定義する

    float4 localposition = float4(input.pos, 1.0f); // 頂点座標
    float4 worldpos = mul(world, localposition); // ワールド座標に変換
    float4 viewpos = mul(view, worldpos); // ビュー座標に変換
    float4 projpos = mul(proj, viewpos); // 投影変換

    output.svpos = projpos; // 投影変換された座標をピクセルシェーダに渡す
    output.color = input.color; // 頂点色をそのままピクセルシェーダに渡す
    output.uv = input.uv;
    return output;
}