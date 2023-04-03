#include "shooting.h"
#include "sharedstruct.h"
#include "descriptorheap.h"
#include "texture2D.h"
#include "app.h"
#include "engine.h"
#include "scene.h"
#include <d3dx12.h>
#include <filesystem>
using namespace DirectX;
using namespace std;

namespace fs = filesystem;

bool Shooting::Shoot(vector<Mesh> &object, vector<OnSceneObject> &on_scene_objects, float init_x, float init_y, float init_z, DirectX::XMFLOAT3 targetposition) {

    if (!gp_scene->AddObjectToScene(object, on_scene_objects, init_x, init_y, init_z)) {
        printf("オブジェクトの配置に失敗");
        return false;
    }

    OnSceneObject &bullet = on_scene_objects[on_scene_objects.size() - 1];

    // 発射時間保存
    bullet.shooted_datetime = chrono::system_clock::now();

    // 弾が飛んでいく方向
    bullet.bullet_vector = targetposition;

    return true;
}

bool Shooting::MoveBullets(UINT currentindex, OnSceneObject &on_scene_object) {
    XMMATRIX &now_world = on_scene_object.constantbuffers[currentindex]->GetPtr<Transform>()->world;

    constexpr float offset = 10.0f;
    const float translation_position_x = now_world.r[3].m128_f32[0] + (on_scene_object.bullet_vector.x * offset);
    const float translation_position_y = now_world.r[3].m128_f32[1] + (on_scene_object.bullet_vector.y * offset);
    const float translation_position_z = now_world.r[3].m128_f32[2] + (on_scene_object.bullet_vector.z * offset);

    now_world = XMMatrixTranslation(translation_position_x, translation_position_y, translation_position_z);
    return true;
}

bool Shooting::ManagementBullets(OnSceneObject &on_scene_object) {
    // 表示する必要のない弾は削除
    // シーンの配列の要素も消えるので、空の要素を削除して詰める

    return true;
}
