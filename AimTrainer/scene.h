#pragma once
#include "sharedstruct.h"

class Scene {
public:
    bool Init(); // 初期化

    void Update(); // 更新処理
    void Draw(); // 描画処理

    bool AddObjectToScene(
        std::vector<Mesh> &object,
        std::vector<OnSceneObject> &on_scene_objects,
        float position_x,
        float position_y,
        float position_z,
        float scale_x = 1.0f,
        float scale_y = 1.0f,
        float scale_z = 1.0f);
};

extern Scene *gp_scene;
