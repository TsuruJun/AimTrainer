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
#include "fbxloader.h"
#include <filesystem>
#include "descriptorheap.h"
#include "texture2D.h"
#include "enemybot.h"
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
ConstantBuffer *gp_constantbuffer[Engine::FRAME_BUFFER_COUNT];
IndexBuffer *gp_indexbuffer;
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;
DescriptorHeap *gp_descriptor_heap;
EnemyBot g_enemy_bot{};

float rotateX = 0.0f;
const char *gp_mode = "+";

const char *gp_model_file = "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\enemy_bot.fbx";
vector<Mesh> g_meshes; // メッシュの配列
vector<VertexBuffer *> gp_vertex_buffers; // メッシュの数分の頂点バッファ
vector<IndexBuffer *> gp_index_buffers; // メッシュの数分のインデックスバッファ
vector<DescriptorHandle *> gp_material_handles; // テクスチャ用のハンドル一覧

// 拡張子を置き換える処理
wstring ReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}

bool Scene::Init() {
    // EnemyBot読み込み
    g_enemy_bot.EnemyLoad(gp_model_file, g_meshes);

    // メッシュの数だけ頂点バッファを用意する
    gp_vertex_buffers.reserve(g_meshes.size());
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto size = sizeof(Vertex) * g_meshes[i].vertices.size();
        auto stride = sizeof(Vertex);
        auto vertices = g_meshes[i].vertices.data();
        auto p_vertex_buffer = new VertexBuffer(size, stride, vertices);
        if (!p_vertex_buffer->IsValid()) {
            printf("頂点バッファの生成に失敗\n");
            return false;
        }

        gp_vertex_buffers.emplace_back(p_vertex_buffer);
    }

    // メッシュの数だけインデックスバッファを用意する
    gp_index_buffers.reserve(g_meshes.size());
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto size = sizeof(uint32_t) * g_meshes[i].indices.size();
        auto indices = g_meshes[i].indices.data();
        auto p_index_buffer = new IndexBuffer(size, indices);
        if (!p_index_buffer->IsValid()) {
            printf("インデックスバッファの生成に失敗");
            return false;
        }

        gp_index_buffers.emplace_back(p_index_buffer);
    }

    auto eyeposition = XMVectorSet(0.0f, 0.0, 10.0, 0.0f); // 視点の位置
    auto targetposition = XMVectorSet(0.0f, 0.0, 0.0, 0.0f);; // 視点を向ける座標
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(60); // 視野角
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

    // マテリアル読み込み
    gp_material_handles.clear();
    gp_descriptor_heap = new DescriptorHeap();

    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto texture_path = ReplaceExtension(g_meshes[i].diffusemap, "tga"); // // もともとはpsdになっていてちょっとめんどかったので、同梱されているtgaを読み込む
        auto main_texture = Texture2D::Get(texture_path);
        auto handle = gp_descriptor_heap->Register(main_texture);
        gp_material_handles.emplace_back(handle);
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
    g_enemy_bot.InitXYZ(0.0f, 0.0f, -10.0f);

    // EnemyBotを往復させる
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto currenttransform = gp_constantbuffer[currentindex]->GetPtr<Transform>(); // 現在のフレーム番号に対応する定数バッファを取得
    g_enemy_bot.RoundTripX(0.0025f, 8.0f, currenttransform);
}

void Scene::Draw() {
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandlist = gp_engine->CommandList(); // コマンドリスト
    auto material_heap = gp_descriptor_heap->GetHeap(); // ディスクリプタヒープ

    // メッシュの数だけインデックス分の描画を処理を行う処理を回す
    for (size_t i = 0; i < g_meshes.size(); ++i) {
        auto vertexbufferview = gp_vertex_buffers[i]->View(); // 頂点バッファビュー
        auto indexbufferview = gp_index_buffers[i]->View(); // インデックスバッファビュー

        commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ルートシグネチャをセット
        commandlist->SetPipelineState(gp_pipelinestate->Get()); // パイプラインステートをセット
        commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffer[currentindex]->GetAddress()); // 定数バッファをセット

        commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
        commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // 頂点バッファをスロット0番を使って1個だけ設定する
        commandlist->IASetIndexBuffer(&indexbufferview); // インデックスバッファをセットする

        commandlist->SetDescriptorHeaps(1, &material_heap); // 使用するディスクリプタヒープをセット
        commandlist->SetGraphicsRootDescriptorTable(1, gp_material_handles[i]->m_handle_GPU); // そのメッシュに対応するディスクリプタテーブルをセット

        commandlist->DrawIndexedInstanced(g_meshes[i].indices.size(), 1, 0, 0, 0); // インデックスの数分を描画する
    }
}
