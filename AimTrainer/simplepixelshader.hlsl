struct VSOutput {
    float4 svpos : SV_POSITION; // ���_�V�F�[�_���痈�����W
    float4 color : COLOR; // ���_�V�F�[�_���痈���F
    float2 uv : TEXCOORD;
};

SamplerState samp : register(s0); // �T���v���[
Texture2D main_tex : register(t0); // �e�N�X�`��

float4 pixel(VSOutput input) : SV_TARGET {
    return main_tex.Sample(samp, input.uv); // �F�����̂܂ܕ\������
}