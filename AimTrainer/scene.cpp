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
        printf("頂点バッファの生成に失敗");
        return false;
    }

    auto eyeposition = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); // 視点の位置
    auto targetposition = XMVectorZero(); // 視点を向ける座標
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(37.5); // 視野角
    auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; ++i) {
        gp_constantbuffer[i] = new ConstantBuffer(sizeof(Transform));
        if (!gp_constantbuffer[i]->IsValid()) {
            printf("変換行列用定数バッファの生成に失敗");
            return false;
        }

        // 変換行列の登録
        auto ptr = gp_constantbuffer[i]->GetPtr<Transform>();
        ptr->world = XMMatrixIdentity();
        ptr->view = XMMatrixLookAtRH(eyeposition, targetposition, upward);
        ptr->proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.f);
    }

    printf("シーンの初期化に成功");
    return true;
}

void Scene::Update() {
}

void Scene::Draw() {
}
