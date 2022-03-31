struct VSOutput {
    float4 svpos : SV_POSITION; // 頂点シェーダから来た座標
    float4 color : COLOR; // 頂点シェーダから来た色
    float2 uv : TEXCOORD;
};

SamplerState samp : register(s0); // サンプラー
Texture2D main_tex : register(t0); // テクスチャ

float4 pixel(VSOutput input) : SV_TARGET {
    return main_tex.Sample(samp, input.uv); // 色をそのまま表示する
}