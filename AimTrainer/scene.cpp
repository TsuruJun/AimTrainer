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
#include "dinputhelper.h"
#include "collision.h"
#include "sound.h"
#include "enemybot.h"
#include <d3dx12.h>
#include <filesystem>
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

// 読み込むモデルをリスト化
vector<const char *> model_list{
    ".\\Model\\fbx\\enemy_bot.fbx",
        ".\\Model\\fbx\\sight.fbx",
        ".\\Model\\fbx\\bullet.fbx"};

Scene *gp_scene;
// シーンのメンバに持たせる
RootSignature *gp_rootsignature;
PipelineState *gp_pipelinestate;
DescriptorHeap *gp_descriptor_heap;
DInputHelper *gp_dinput_helper;

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

Shooting g_shooting;
Sound g_sound;
EnemyBot *g_enemybot;

vector<OnSceneObject> on_scene_objects;

// 拡張子を置き換える処理
wstring ReplaceExtension(const wstring &origin, const char *extention) {
    fs::path path = origin.c_str();
    return path.replace_extension(extention).c_str();
}

bool Scene::AddObjectToScene(
    vector<Mesh> &object,
    vector<OnSceneObject> &on_scene_objects,
    float position_x,
    float position_y,
    float position_z,
    float scale_x,
    float scale_y,
    float scale_z) {
    OnSceneObject on_scene_object = {};

    // オブジェクトのポインタを保存
    for (size_t i = 0; i < object.size(); ++i) {
        on_scene_object.object.emplace_back(&object[i]);
    }

    // バウンディングボックスを計算
    Collision collision;
    BoundingBox boundingbox = collision.ComputeBoundingBox(object);
    boundingbox.min.x += position_x;
    boundingbox.min.y += position_y;
    boundingbox.min.z += position_z;
    on_scene_object.boundingbox = boundingbox;

    // メッシュの数だけ頂点バッファを用意する
    for (size_t i = 0; i < object.size(); ++i) {
        auto size = sizeof(Vertex) * object[i].vertices.size();
        auto stride = sizeof(Vertex);
        auto vertices = object[i].vertices.data();
        auto *p_vertex_buffer = new VertexBuffer(size, stride, vertices);
        if (!p_vertex_buffer->IsValid()) {
            printf("頂点バッファの生成に失敗\n");
            return false;
        }

        on_scene_object.vertex_buffer.emplace_back(p_vertex_buffer);
    }

    // メッシュの数だけインデックスバッファを用意する
    for (size_t i = 0; i < object.size(); ++i) {
        auto size = sizeof(uint32_t) * object[i].indices.size();
        auto indices = object[i].indices.data();
        auto *p_index_buffer = new IndexBuffer(size, indices);
        if (!p_index_buffer->IsValid()) {
            printf("インデックスバッファの生成に失敗");
            return false;
        }

        on_scene_object.index_buffer.emplace_back(p_index_buffer);
    }

    // マテリアル読み込み
    //gp_descriptor_heap = new DescriptorHeap();

    //for (size_t i = 0; i < object.size(); ++i) {
    //    auto texture_path = ReplaceExtension(object[i].diffusemap, "tga"); // // もともとはpsdになっていてちょっとめんどかったので、同梱されているtgaを読み込む
    //    auto main_texture = Texture2D::Get(texture_path);
    //    auto handle = gp_descriptor_heap->Register(main_texture);
    //    on_scene_object.material_handle.emplace_back(handle);
    //}

    // カメラ設定
    const XMFLOAT3 eyeposition = {0.0f, 0.0f, 0.0f}; // 視点の位置
    const XMFLOAT3 targetposition = {0.0f + eyeposition.x, eyeposition.y, 1.0f + eyeposition.z};// 視点を向ける座標
    const XMFLOAT3 upward = {0.0f, 1.0f, 0.0f}; // 上方向を表すベクトル
    constexpr auto fov = XMConvertToRadians(52.0f); // 視野角
    constexpr auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

    for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; ++i) {
        on_scene_object.constantbuffers.emplace_back(new ConstantBuffer(sizeof(Transform)));
        if (!on_scene_object.constantbuffers[i]->IsValid()) {
            printf("変換行列用定数バッファの生成に失敗");
            return false;
        }

        // 変換行列の登録
        auto ptr = on_scene_object.constantbuffers[i]->GetPtr<Transform>();
        ptr->world = XMMatrixIdentity() * XMMatrixScaling(scale_x, scale_y, scale_z) * XMMatrixTranslation(position_x, position_y, position_z);
        ptr->view = XMMatrixLookAtLH(XMLoadFloat3(&eyeposition), XMLoadFloat3(&targetposition), XMLoadFloat3(&upward));
        ptr->proj = XMMatrixPerspectiveFovLH(fov, aspect, 0.3f, 1000.f);
    }

    on_scene_objects.emplace_back(on_scene_object);

    return true;
}

bool Scene::Init() {
    FbxLoader loader;
    gp_dinput_helper = new DInputHelper();

    // モデルを読み込む
    for (int i = 0; i < model_list.size(); ++i) {
        // モデル読み込み
        if (!loader.FbxLoad(model_list[i])) {
            printf("モデルの読み込みに失敗");
            return false;
        }
        g_objects.emplace_back(loader.GetMeshes());
        loader.ClearMeshes();
    }

    // ボット初期化
    AddObjectToScene(g_objects[0], on_scene_objects, 0.0f, -2.5f, 10.0f);
    // EnemyBot初期化
    g_enemybot = new EnemyBot(0.0f, -2.5f, 10.0f);
    // サイト初期化
    AddObjectToScene(g_objects[1], on_scene_objects, 0.0f, 0.0f, 3.0f);

    gp_rootsignature = new RootSignature();
    if (!gp_rootsignature->IsValid()) {
        printf("ルートシグネチャの生成に失敗");
        return false;
    }

    gp_pipelinestate = new PipelineState();
    gp_pipelinestate->SetInputLayout(Vertex::inputlayout);
    gp_pipelinestate->SetRootSignature(gp_rootsignature->Get());
    gp_pipelinestate->SetVertexShader(L"..\\x64\\Debug\\simplevertexshader.cso");
    gp_pipelinestate->SetPixelShader(L"..\\x64\\Debug\\simplepixelshader.cso");
    gp_pipelinestate->Create();
    if (!gp_pipelinestate->IsValid()) {
        printf("パイプラインステートの生成に失敗");
        return false;
    }

    printf("シーンの初期化に成功");

    // サウンド初期化
    g_sound.InitSound();
    WCHAR sound_file_name[] = L".\\Sound\\hit.wav";
    g_sound.LoadSound(sound_file_name);

    return true;
}

void Scene::Update() {
    const auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する

    // bot移動
    g_enemybot->RoundTripX(0.02f, 10.0f, on_scene_objects[0].constantbuffers[currentindex]->GetPtr<Transform>(), on_scene_objects[0].boundingbox);

    // マウス座標取得
    POINT point{};
    GetCursorPos(&point);
    ScreenToClient(gp_engine->GetHwnd(), &point);

    // 中心からのマウスの差分を取得
    const float diff_x = (static_cast<float>(point.x) - static_cast<float>(WINDOW_WIDTH) / 2) / 500;
    const float diff_y = (static_cast<float>(WINDOW_HEIGHT) / 2 - static_cast<float>(point.y)) / 500; // スクリーン座標ではyの方向が逆

    // サイトをカメラに合わせて動かす
    on_scene_objects[1].constantbuffers[currentindex]->GetPtr<Transform>()->world = XMMatrixTranslation(0.0f, 0.0f, 3.0f) * XMMatrixRotationRollPitchYaw(-diff_y, diff_x, 0.0f);

    const float sight_position_x = on_scene_objects[1].constantbuffers[currentindex]->GetPtr<Transform>()->world.r[3].m128_f32[0];
    const float sight_position_y = on_scene_objects[1].constantbuffers[currentindex]->GetPtr<Transform>()->world.r[3].m128_f32[1];
    const float sight_position_z = on_scene_objects[1].constantbuffers[currentindex]->GetPtr<Transform>()->world.r[3].m128_f32[2];

    // 弾とボットの当たり判定
    Collision collision;
    for (size_t i = 0; i < on_scene_objects.size(); ++i) {
        if (on_scene_objects[i].is_bullet) {
            XMMATRIX &now_world = on_scene_objects[i].constantbuffers[currentindex]->GetPtr<Transform>()->world;

            const float current_position_x = now_world.r[3].m128_f32[0];
            const float current_position_y = now_world.r[3].m128_f32[1];
            const float current_position_z = now_world.r[3].m128_f32[2];
            XMFLOAT3 point_position = {current_position_x, current_position_y, current_position_z};

            const bool ishit = collision.isCollisionPointToBoundingBox(point_position, on_scene_objects[0].boundingbox);

            if (ishit) {
                on_scene_objects[i].hit = ishit;
                g_sound.PlaySoundFile();
            }
        }
    }

    // 弾を削除
    g_shooting.ManagementBullets(on_scene_objects);

    // 弾を動かす
    for (size_t i = 0; i < on_scene_objects.size(); ++i) {
        if (on_scene_objects[i].is_bullet) {
            g_shooting.MoveBullet(currentindex, on_scene_objects[i]);
        }
    }

    // 弾を撃つ
    if (gp_dinput_helper->isLeftClick()) {
        g_shooting.Shoot(
            g_objects[2],
            on_scene_objects,
            {sight_position_x * 0.1f, sight_position_y * 0.1f, sight_position_z * 0.1f},
            sight_position_x,
            sight_position_y,
            sight_position_z,
            0.2f,
            0.2f,
            0.2f);
    }

    // マウスの座標をカメラの向きに反映
    const XMFLOAT3 eyeposition = {0.0f, 0.0f, 0.0f};
    const XMFLOAT3 targetposition = {eyeposition.x, eyeposition.y, 1.0f + eyeposition.z};
    const XMFLOAT3 upward = {0.0f, 1.0f, 0.0f};

    const XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&eyeposition), XMLoadFloat3(&targetposition), XMLoadFloat3(&upward)) * XMMatrixRotationRollPitchYaw(diff_y, -diff_x, 0.0f);

    for (size_t i = 0; i < on_scene_objects.size(); ++i) {
        on_scene_objects[i].constantbuffers[currentindex]->GetPtr<Transform>()->view = view;
    }
}

void Scene::Draw() {
    const auto currentindex = gp_engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
    const auto commandlist = gp_engine->CommandList(); // コマンドリスト
    //auto material_heap = gp_descriptor_heap->GetHeap(); // ディスクリプタヒープ

    // モデルの数だけ描画
    for (int count = 0; count < on_scene_objects.size(); ++count) {
        // メッシュの数だけインデックス分の描画を処理を行う処理を回す
        for (size_t i = 0; i < on_scene_objects[count].object.size(); ++i) {
            const auto vertexbufferview = on_scene_objects[count].vertex_buffer[i]->View(); // 頂点バッファビュー
            const auto indexbufferview = on_scene_objects[count].index_buffer[i]->View(); // インデックスバッファビュー

            commandlist->SetGraphicsRootSignature(gp_rootsignature->Get()); // ルートシグネチャをセット
            commandlist->SetPipelineState(gp_pipelinestate->Get()); // パイプラインステートをセット
            commandlist->SetGraphicsRootConstantBufferView(0, on_scene_objects[count].constantbuffers[currentindex]->GetAddress()); // 定数バッファをセット

            commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
            commandlist->IASetVertexBuffers(0, 1, &vertexbufferview); // 頂点バッファをスロット0番を使って1個だけ設定する
            commandlist->IASetIndexBuffer(&indexbufferview); // インデックスバッファをセットする

            //commandlist->SetDescriptorHeaps(1, &material_heap); // 使用するディスクリプタヒープをセット
            //commandlist->SetGraphicsRootDescriptorTable(1, on_scene_objects[count].material_handle[i]->m_handle_GPU); // そのメッシュに対応するディスクリプタテーブルをセット

            commandlist->DrawIndexedInstanced(on_scene_objects[count].object[i]->indices.size(), 1, 0, 0, 0); // インデックスの数分を描画する
        }
    }
}
