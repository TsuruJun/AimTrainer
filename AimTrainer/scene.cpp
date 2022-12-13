#include "scene.h"
#include "app.h"
#include "constantbuffer.h"
#include "descriptorheap.h"
#include "engine.h"
#include "fbxloader.h"
#include "indexbuffer.h"
#include "pipelinestate.h"
#include "rootsignature.h"
#include "sharedstruct.h"
#include "texture2D.h"
#include "vertexbuffer.h"
#include <d3dx12.h>
#include <filesystem>
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

// 読み込むモデルをリスト化
vector<const char *> model_list{
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\enemy_bot.fbx",
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\bullet.fbx",
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\sight.fbx"};

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
vector<ConstantBuffer *> gp_constantbuffer{Engine::FRAME_BUFFER_COUNT * model_list.size()};
IndexBuffer *gp_indexbuffer;
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;
DescriptorHeap *gp_descriptor_heap;

float rotateX = 0.0f;

// プレイヤーの位置
float position_x = 0.0f;
float position_y = 0.0f;
float position_z = -0.0f;

vector<vector<Mesh>> g_objects; // メッシュの配列
vector<vector<VertexBuffer *>> gp_vertex_buffers; // メッシュの数分の頂点バッファ
vector<vector<IndexBuffer *>> gp_index_buffers; // メッシュの数分のインデックスバッファ
vector<vector<DescriptorHandle *>> gp_material_handles; // テクスチャ用のハンドル一覧

// 拡張子を置き換える処理
wstring ReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}

bool Scene::Init() {
    FbxLoader loader;

    // 各バッファのサイズを初期化
    gp_vertex_buffers.resize(model_list.size());
    gp_index_buffers.resize(model_list.size());
    gp_material_handles.resize(model_list.size());

    // モデルごとに読み込む
    for (int count = 0; count < model_list.size(); ++count) {
        // モデル読み込み
        if (!loader.FbxLoad(model_list[count])) {
            printf("モデルの読み込みに失敗");
            return false;
        }
        g_objects.emplace_back(loader.GetMeshes());
        loader.ClearMeshes();

        // メッシュの数だけ頂点バッファを用意する
        gp_vertex_buffers[count].reserve(g_objects[count].size());
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto size = sizeof(Vertex) * g_objects[count][i].vertices.size();
            auto stride = sizeof(Vertex);
            auto vertices = g_objects[count][i].vertices.data();
            auto p_vertex_buffer = new VertexBuffer(size, stride, vertices);
            if (!p_vertex_buffer->IsValid()) {
                printf("頂点バッファの生成に失敗\n");
                return false;
            }

            gp_vertex_buffers[count].emplace_back(p_vertex_buffer);
        }

        // メッシュの数だけインデックスバッファを用意する
        gp_index_buffers[count].reserve(g_objects[count].size());
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto size = sizeof(uint32_t) * g_objects[count][i].indices.size();
            auto indices = g_objects[count][i].indices.data();
            auto p_index_buffer = new IndexBuffer(size, indices);
            if (!p_index_buffer->IsValid()) {
                printf("インデックスバッファの生成に失敗");
                return false;
            }

            gp_index_buffers[count].emplace_back(p_index_buffer);
        }

        // マテリアル読み込み
        gp_material_handles[count].clear();
        gp_descriptor_heap = new DescriptorHeap();

        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto texture_path = ReplaceExtension(g_objects[count][i].diffusemap, "tga"); // // もともとはpsdになっていてちょっとめんどかったので、同梱されているtgaを読み込む
            auto main_texture = Texture2D::Get(texture_path);
            auto handle = gp_descriptor_heap->Register(main_texture);
            gp_material_handles[count].emplace_back(handle);
        }
    }

    // カメラ設定
    auto eyeposition = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f); // 視点の位置
    auto targetposition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);// 視点を向ける座標
    auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(100.0f); // 視野角
    auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT * model_list.size(); ++i) {
        gp_constantbuffer[i] = new ConstantBuffer(sizeof(Transform));
        if (!gp_constantbuffer[i]->IsValid()) {
            printf("変換行列用定数バッファの生成に失敗");
            return false;
        }

        // 変換行列の登録
        auto ptr = gp_constantbuffer[i]->GetPtr<Transform>();
        if (i % model_list.size() == 0) { // enemy_bot
            ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0, 0, -10.0f);
        } else if (i % model_list.size() == 1) { // bullet
            ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0, 0, 4.0f);
        } else { // sight
            ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0, 0, 8.0f);
        }
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
    rotateX += 0.001f;
    // EnemyBotを往復させる
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する

    gp_constantbuffer[currentindex * model_list.size() + 0]->GetPtr<Transform>()->world = XMMatrixTranslation(rotateX, 0, 0); // enemy_bot
    gp_constantbuffer[currentindex * model_list.size() + 1]->GetPtr<Transform>()->world = XMMatrixTranslation(0, 0, -rotateX); // bullet

    // WASDで移動
    // 前
    if (GetKeyState('W') & 0x80) {
        position_z += 0.05f;
    }
    // 後ろ
    if (GetKeyState('S') & 0x80) {
        position_z -= 0.05f;
    }
    // 左
    if (GetKeyState('A') & 0x80) {
        position_x += 0.05f;
    }
    // 右
    if (GetKeyState('D') & 0x80) {
        position_x -= 0.05f;
    }

    // オブジェクトを動かして移動したように見せる
    gp_constantbuffer[currentindex * model_list.size() + 0]->GetPtr<Transform>()->world = XMMatrixTranslation(position_x, 0, position_z - 10.0f);
    gp_constantbuffer[currentindex * model_list.size() + 1]->GetPtr<Transform>()->world = XMMatrixTranslation(position_x, 0, position_z + 4.0f);
    gp_constantbuffer[currentindex * model_list.size() + 2]->GetPtr<Transform>()->world = XMMatrixTranslation(position_x, 0, position_z + 8.0f);
}

void Scene::Draw() {
    auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    auto commandlist = gp_engine->CommandList(); // コマンドリスト
    auto material_heap = gp_descriptor_heap->GetHeap(); // ディスクリプタヒープ

    // モデルの数だけ描画
    for (int count = 0; count < g_objects.size(); ++count) {
        // メッシュの数だけインデックス分の描画を処理を行う処理を回す
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto vertexbufferview = gp_vertex_buffers[count][i]->View(); // 頂点バッファビュー
            auto indexbufferview = gp_index_buffers[count][i]->View(); // インデックスバッファビュー

            commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ルートシグネチャをセット
            commandlist->SetPipelineState(gp_pipelinestate->Get()); // パイプラインステートをセット
            commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffer[currentindex * model_list.size() + count]->GetAddress()); // 定数バッファをセット

            commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
            commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // 頂点バッファをスロット0番を使って1個だけ設定する
            commandlist->IASetIndexBuffer(&indexbufferview); // インデックスバッファをセットする

            commandlist->SetDescriptorHeaps(1, &material_heap); // 使用するディスクリプタヒープをセット
            commandlist->SetGraphicsRootDescriptorTable(1, gp_material_handles[count][i]->m_handle_GPU); // そのメッシュに対応するディスクリプタテーブルをセット

            commandlist->DrawIndexedInstanced(g_objects[count][i].indices.size(), 1, 0, 0, 0); // インデックスの数分を描画する
        }
    }
}
