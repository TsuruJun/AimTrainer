#include "scene.h"
#include "engine.h"
#include "app.h"
#include <d3dx12.h>
#include "sharedstruct.h"
#include "vertexbuffer.h"
#include "constantbuffer.h"
using namespace DirectX;

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
ConstantBuffer *gp_constantbuffer[Engine::FRAME_BUFFER_COUNT];

bool Scene::Init() {
    Vertex vertices[3] = {};
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
    vertices[0].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    vertices[1].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

    vertices[2].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    auto vertexsize = sizeof(Vertex) * std::size(vertices);
    auto vertexstride = sizeof(Vertex);
    gp_vertexbuffer = new VertexBuffer(vertexsize, vertexstride, vertices);
    if (!gp_vertexbuffer->IsValid()) {
        printf("���_�o�b�t�@�̐����Ɏ��s");
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

    printf("�V�[���̏������ɐ���");
    return true;
}

void Scene::Update() {
}

void Scene::Draw() {
}
