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
        printf("頂点バッファの生成に失敗");
        return false;
    }

    uint32_t indices[] = {0, 1, 2, 0, 2, 3}; // これに書かれている順序で描画する

    // インデックスバッファの設定
    auto size = sizeof(uint32_t) * std::size(indices);
    gp_indexbuffer = new IndexBuffer(size, indices);
    if (!gp_indexbuffer->IsValid()) {
        printf("インデックスバッファの生成に失敗");
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

    gp_rootsignature = new RootSignature();
    if (!gp_rootsignature->IsValid()) {
        printf("ルートシグネチャの生成に失敗");
        return false;
    }

    gp_pipelinestate = new PipelineState();
    gp_pipelinestate->SetInputLayout(Vertex::inputlayout);
    gp_pipelinestate->SetRootSignature(gp_rootsignature->Get());
    gp_pipelinestate->SetVertexShader(L"../x64/Debug/simplevertexshader.cso");
    gp_pipelinestate->SetPixelShader(L"../x64/Debug/simplepixelshader.cso");
    gp_pipelinestate->Create();
    if (!gp_pipelinestate->IsValid()) {
        printf("パイプラインステートの生成に失敗");
        return false;
    }

    printf("シーンの初期化に成功");
    return true;
}

void Scene::Update() {
}

void Scene::Draw() {
    rotateY += 0.002f;
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto currenttransform = gp_constantbuffer[currentindex]->GetPtr<Transform>(); // 現在のフレーム番号に対応する定数バッファを取得
    currenttransform->world = DirectX::XMMatrixRotationY(rotateY); // Y軸で回転させる
    auto commandlist = gp_engine->CommandList(); // コマンドリスト
    auto vertexbufferview = gp_vertexbuffer->View(); // 頂点バッファビュー
    auto indexbufferview = gp_indexbuffer->View(); // インデックスバッファビュー

    commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ルートシグネチャをセット
    commandlist->SetPipelineState(gp_pipelinestate->Get()); // パイプラインステートをセット
    commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffer[currentindex]->GetAddress()); // 定数バッファをセット
    commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
    commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // 頂点バッファをスロット0番を使って1個だけ設定する
    commandlist->IASetIndexBuffer(&indexbufferview); // インデックスバッファをセットする

    commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0); // 3個の頂点を描画する
}
