struct VSOutput {
    float4 svpos : SV_POSITION; // 頂点シェーダから来た座標
    float4 color : COLOR; // 頂点シェーダから来た色
};

float4 pixel(VSOutput input) : SV_TARGET {
    return input.color; // 色をそのまま表示する
}