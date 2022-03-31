cbuffer Transform : register(b0) {
    float4x4 world; // ���[���h�s��
    float4x4 view; // �r���[�s��
    float4x4 proj; // ���e�s��
}

struct VSInput {
    float3 pos : POSITION; // ���_���W
    float3 normal : NORMAL; // �@��
    float2 uv : TEXCOORD; // uv
    float3 tangent : TANGENT; // �ڋ��
    float4 color : COLOR; // ���_�F
};

struct VSOutput {
    float4 svpos : SV_POSITION; // �ϊ����ꂽ���W
    float4 color : COLOR; // �ϊ����ꂽ�F
    float2 uv : TEXCOORD;
};

VSOutput vert(VSInput input) {
    VSOutput output = (VSOutput)0; // �A�E�g�v�b�g�\���̂��`����

    float4 localposition = float4(input.pos, 1.0f); // ���_���W
    float4 worldpos = mul(world, localposition); // ���[���h���W�ɕϊ�
    float4 viewpos = mul(view, worldpos); // �r���[���W�ɕϊ�
    float4 projpos = mul(proj, viewpos); // ���e�ϊ�

    output.svpos = projpos; // ���e�ϊ����ꂽ���W���s�N�Z���V�F�[�_�ɓn��
    output.color = input.color; // ���_�F�����̂܂܃s�N�Z���V�F�[�_�ɓn��
    output.uv = input.uv;
    return output;
}