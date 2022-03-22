#include "scene.h"
#include "engine.h"
#include "app.h"
#include <d3dx12.h>
#include "sharedstruct.h"
#include "vertexbuffer.h"
#include "constantbuffer.h"
#include "rootsignature.h"
#include "pipelinestate.h"
#include "indexbuffer.h"
using namespace DirectX;

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
ConstantBuffer *gp_constantbuffer[Engine::FRAME_BUFFER_COUNT];
IndexBuffer *gp_indexbuffer;
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;

float rotateY = 0.0f;

bool Scene::Init() {
    Vertex vertices[4] = {};
    vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
    vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    vertices[3].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[3].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

    auto vertexsize = sizeof(Vertex) * std::size(vertices);
    auto vertexstride = sizeof(Vertex);
    gp_vertexbuffer = new VertexBuffer(vertexsize, vertexstride, vertices);
    if (!gp_vertexbuffer->IsValid()) {
        printf("���_�o�b�t�@�̐����Ɏ��s");
        return false;
    }

    uint32_t indices[] = {0, 1, 2, 0, 2, 3}; // ����ɏ�����Ă��鏇���ŕ`�悷��

    // �C���f�b�N�X�o�b�t�@�̐ݒ�
    auto size = sizeof(uint32_t) * std::size(indices);
    gp_indexbuffer = new IndexBuffer(size, indices);
    if (!gp_indexbuffer->IsValid()) {
        printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s");
        return false;
    }

    auto eyeposition = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); // ���_�̈ʒu
    auto targetposition = XMVectorZero(); // ���_����������W
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // �������\���x�N�g��
    constexpr auto fov = XMConvertToRadians(37.5); // ����p
    auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // �A�X�y�N�g��

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; ++i) {
        gp_constantbuffer[i] = new ConstantBuffer(sizeof(Transform));
        if (!gp_constantbuffer[i]->IsValid()) {
            printf("�ϊ��s��p�萔�o�b�t�@�̐����Ɏ��s");
            return false;
        }

        // �ϊ��s��̓o�^
        auto ptr = gp_constantbuffer[i]->GetPtr<Transform>();
        ptr->world = XMMatrixIdentity();
        ptr->view = XMMatrixLookAtRH(eyeposition, targetposition, upward);
        ptr->proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.f);
    }

    gp_rootsignature = new RootSignature();
    if (!gp_rootsignature->IsValid()) {
        printf("���[�g�V�O�l�`���̐����Ɏ��s");
        return false;
    }

    gp_pipelinestate = new PipelineState();
    gp_pipelinestate->SetInputLayout(Vertex::inputlayout);
    gp_pipelinestate->SetRootSignature(gp_rootsignature->Get());
    gp_pipelinestate->SetVertexShader(L"../x64/Debug/simplevertexshader.cso");
    gp_pipelinestate->SetPixelShader(L"../x64/Debug/simplepixelshader.cso");
    gp_pipelinestate->Create();
    if (!gp_pipelinestate->IsValid()) {
        printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s");
        return false;
    }

    printf("�V�[���̏������ɐ���");
    return true;
}

void Scene::Update() {
}

void Scene::Draw() {
    rotateY += 0.002f;
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
    auto currenttransform = gp_constantbuffer[currentindex]->GetPtr<Transform>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾
    currenttransform->world = DirectX::XMMatrixRotationY(rotateY); // Y���ŉ�]������
    auto commandlist = gp_engine->CommandList(); // �R�}���h���X�g
    auto vertexbufferview = gp_vertexbuffer->View(); // ���_�o�b�t�@�r���[
    auto indexbufferview = gp_indexbuffer->View(); // �C���f�b�N�X�o�b�t�@�r���[

    commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ���[�g�V�O�l�`�����Z�b�g
    commandlist->SetPipelineState(gp_pipelinestate->Get()); // �p�C�v���C���X�e�[�g���Z�b�g
    commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffer[currentindex]->GetAddress()); // �萔�o�b�t�@���Z�b�g
    commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`��`�悷��ݒ�ɂ���
    commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // ���_�o�b�t�@���X���b�g0�Ԃ��g����1�����ݒ肷��
    commandlist->IASetIndexBuffer(&indexbufferview); // �C���f�b�N�X�o�b�t�@���Z�b�g����

    commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0); // 3�̒��_��`�悷��
}
