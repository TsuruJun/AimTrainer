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
#include "shooting.h"
#include <d3dx12.h>
#include <filesystem>
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

// 読み込むモデルをリスト化
vector<const char *> model_list{
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\enemy_bot.fbx",
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\sight.fbx",
    "C:\\Users\\TsuruJun\\source\\repos\\Model\\fbx\\bullet.fbx"};

Scene *gp_scene;
VertexBuffer *gp_vertexbuffer;
vector<ConstantBuffer *> gp_constantbuffers(Engine::FRAME_BUFFER_COUNT * 2);
IndexBuffer *gp_indexbuffer;
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;
DescriptorHeap *gp_descriptor_heap;

float rotateX = 0.0f;
float yaw = 0.0f;
float pitch = 0.0f;

// プレイヤーの位置
float position_x = 0.0f;
float position_y = 0.0f;
float position_z = 0.0f;

// マウスの座標
int mouse_position_x = 0;
int mouse_position_y = 0;

vector<vector<Mesh>> g_objects; // メッシュの配列
vector<vector<VertexBuffer *>> g_vertex_buffers; // メッシュの数分の頂点バッファ
vector<vector<IndexBuffer *>> g_index_buffers; // メッシュの数分のインデックスバッファ
vector<vector<DescriptorHandle *>> g_material_handles; // テクスチャ用のハンドル一覧

Shooting g_shooting;

// 拡張子を置き換える処理
wstring ReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}


bool Scene::Init() {
    FbxLoader loader;

    // 各バッファのサイズを初期化
    g_vertex_buffers.resize(2);
    g_index_buffers.resize(2);
    g_material_handles.resize(2);

    // モデルごとに読み込む
    for (int count = 0; count < 2; ++count) {
        // モデル読み込み
        if (!loader.FbxLoad(model_list[count])) {
            printf("モデルの読み込みに失敗");
            return false;
        }
        g_objects.emplace_back(loader.GetMeshes());
        loader.ClearMeshes();

        // メッシュの数だけ頂点バッファを用意する
        g_vertex_buffers[count].reserve(g_objects[count].size());
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto size = sizeof(Vertex) * g_objects[count][i].vertices.size();
            auto stride = sizeof(Vertex);
            auto vertices = g_objects[count][i].vertices.data();
            auto p_vertex_buffer = new VertexBuffer(size, stride, vertices);
            if (!p_vertex_buffer->IsValid()) {
                printf("頂点バッファの生成に失敗\n");
                return false;
            }

            g_vertex_buffers[count].emplace_back(p_vertex_buffer);
        }

        // メッシュの数だけインデックスバッファを用意する
        g_index_buffers[count].reserve(g_objects[count].size());
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto size = sizeof(uint32_t) * g_objects[count][i].indices.size();
            auto indices = g_objects[count][i].indices.data();
            auto p_index_buffer = new IndexBuffer(size, indices);
            if (!p_index_buffer->IsValid()) {
                printf("インデックスバッファの生成に失敗");
                return false;
            }

            g_index_buffers[count].emplace_back(p_index_buffer);
        }

        // マテリアル読み込み
        gp_descriptor_heap = new DescriptorHeap();

        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            auto texture_path = ReplaceExtension(g_objects[count][i].diffusemap, "tga"); // // もともとはpsdになっていてちょっとめんどかったので、同梱されているtgaを読み込む
            auto main_texture = Texture2D::Get(texture_path);
            auto handle = gp_descriptor_heap->Register(main_texture);
            g_material_handles[count].emplace_back(handle);
        }
    }

    // カメラ設定
    const XMFLOAT3 eyeposition = {0.0f, 1.0f, 0.0f}; // 視点の位置
    const XMFLOAT3 targetposition = {0.0f + eyeposition.x, 1.0f, 1.0f + eyeposition.z};// 視点を向ける座標
    const XMFLOAT3 upward = {0.0f, 1.0f, 0.0f}; // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(52.0f); // 視野角
    constexpr auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT * 2; ++i) {
        gp_constantbuffers[i] = new ConstantBuffer(sizeof(Transform));
        if (!gp_constantbuffers[i]->IsValid()) {
            printf("変換行列用定数バッファの生成に失敗");
            return false;
        }

        // 変換行列の登録
        auto ptr = gp_constantbuffers[i]->GetPtr<Transform>();
        if (i == 0 || i == 1) { // enemy_bot
            ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 0.0f, 100.0f);
        } else if (i == 2 || i == 3) { // sight
            ptr->world = XMMatrixIdentity() * XMMatrixTranslation(0.0f, 1.0f, 3.0f);
        }
        ptr->view = XMMatrixLookAtLH(XMLoadFloat3(&eyeposition), XMLoadFloat3(&targetposition), XMLoadFloat3(&upward));
        ptr->proj = XMMatrixPerspectiveFovLH(fov, aspect, 0.3f, 1000.f);
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

    //// 試しに弾丸を削除
    //g_objects.pop_back();
    //g_vertex_buffers.pop_back();
    //g_index_buffers.pop_back();
    //g_material_handles.pop_back();
    //gp_constantbuffers.erase(gp_constantbuffers.begin() + 2);
    //gp_constantbuffers.pop_back();
    return true;
}

void Scene::Update() {
    const auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する

    // マウス座標取得
    POINT point{};
    GetCursorPos(&point);
    ScreenToClient(gp_engine->GetHwnd(), &point);

    // 中心からのマウスの差分を取得
    const float diff_x = (static_cast<float>(point.x) - static_cast<float>(WINDOW_WIDTH) / 2) / 1000;
    const float diff_y = (static_cast<float>(point.y) - static_cast<float>(WINDOW_HEIGHT) / 2) / 1000;

    // マウスの座標をカメラの向きに反映
    const XMFLOAT3 eyeposition = {0.0f, 1.0f, 0.0f};
    const XMFLOAT3 targetposition = {diff_x + eyeposition.x, 1.0f - diff_y, 1.0f + eyeposition.z};
    const XMFLOAT3 upward = {0.0f, 1.0f, 0.0f};

    //// カメラの移動
    //if (GetKeyState('W') & 0x80) {
    //    position_z += 0.05f;
    //}
    //// 後ろ
    //if (GetKeyState('S') & 0x80) {
    //    position_z -= 0.05f;
    //}
    //// 左
    //if (GetKeyState('A') & 0x80) {
    //    position_x -= 0.05f;
    //}
    //// 右
    //if (GetKeyState('D') & 0x80) {
    //    position_x += 0.05f;
    //}

    //eyeposition.x += position_x;
    //eyeposition.z += position_z;

    const XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eyeposition), XMLoadFloat3(&targetposition), XMLoadFloat3(&upward));

    gp_constantbuffers[currentindex + 0]->GetPtr<Transform>()->view = view; // enemy_bot
    gp_constantbuffers[currentindex + 2]->GetPtr<Transform>()->view = view; // sight

    // カメラに合わせてサイトを移動
    // TODO: ローカル座標で動かしてワールド座標に変換し入れたい
    //gp_constantbuffers[currentindex * g_objects.size() + 1]->GetPtr<Transform>()->world = XMMatrixTranslation(diff_x, 1.0f - diff_y, 2.0f + eyeposition.z); // sight
}

void Scene::Draw() {
    const auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    const auto commandlist = gp_engine->CommandList(); // コマンドリスト
    auto material_heap = gp_descriptor_heap->GetHeap(); // ディスクリプタヒープ

    // モデルの数だけ描画
    for (int count = 0; count < 2; ++count) {
        // メッシュの数だけインデックス分の描画を処理を行う処理を回す
        for (size_t i = 0; i < g_objects[count].size(); ++i) {
            const auto vertexbufferview = g_vertex_buffers[count][i]->View(); // 頂点バッファビュー
            const auto indexbufferview = g_index_buffers[count][i]->View(); // インデックスバッファビュー

            commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ルートシグネチャをセット
            commandlist->SetPipelineState(gp_pipelinestate->Get()); // パイプラインステートをセット
            commandlist->SetGraphicsRootConstantBufferView(0, gp_constantbuffers[currentindex + count * 2]->GetAddress()); // 定数バッファをセット

            commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
            commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // 頂点バッファをスロット0番を使って1個だけ設定する
            commandlist->IASetIndexBuffer(&indexbufferview); // インデックスバッファをセットする

            commandlist->SetDescriptorHeaps(1, &material_heap); // 使用するディスクリプタヒープをセット
            commandlist->SetGraphicsRootDescriptorTable(1, g_material_handles[count][i]->m_handle_GPU); // そのメッシュに対応するディスクリプタテーブルをセット

            commandlist->DrawIndexedInstanced(g_objects[count][i].indices.size(), 1, 0, 0, 0); // インデックスの数分を描画する
        }
    }
}
