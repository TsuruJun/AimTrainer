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

bool Shooting::Shoot(
    vector<Mesh> &object,
    vector<OnSceneObject> &on_scene_objects,
    DirectX::XMFLOAT3 targetposition,
    float position_x,
    float position_y,
    float position_z,
    float scale_x,
    float scale_y,
    float scale_z) {

    if (!gp_scene->AddObjectToScene(object, on_scene_objects, position_x, position_y, position_z, scale_x, scale_y, scale_z)) {
        printf("オブジェクトの配置に失敗");
        return false;
    }

    OnSceneObject &bullet = on_scene_objects[on_scene_objects.size() - 1];

    // オブジェクトが弾であることを示す
    bullet.is_bullet = true;

    // 発射時間保存
    bullet.shooted_datetime = chrono::system_clock::now().time_since_epoch().count();

    // 弾が飛んでいく方向
    bullet.bullet_vector = targetposition;

    return true;
}

bool Shooting::MoveBullet(UINT currentindex, OnSceneObject &on_scene_object) {
    XMMATRIX &now_world = on_scene_object.constantbuffers[currentindex]->GetPtr<Transform>()->world;

    now_world *= XMMatrixTranslation(on_scene_object.bullet_vector.x, on_scene_object.bullet_vector.y, on_scene_object.bullet_vector.z);
    return true;
}

void Shooting::ManagementBullets(vector<OnSceneObject> &on_scene_objects) {
    // 表示する必要のない弾は削除
    auto now = chrono::system_clock::now().time_since_epoch().count();

    for (auto itr = on_scene_objects.begin(); itr != on_scene_objects.end();) {
        // 弾が作成されて1秒以上たつまたは弾が当たる
        if (itr->is_bullet && ((now - itr->shooted_datetime) > 3000000 || itr->hit)) {
            // 削除
            itr = on_scene_objects.erase(itr); // 次のイテレータが返される
        } else {
            ++itr;
        }
    }
}
