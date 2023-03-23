#pragma once
#include "sharedstruct.h"

class Scene {
public:
    bool Init(); // 初期化

    void Update(); // 更新処理
    void Draw(); // 描画処理

private:
    bool AddObjectToScene(std::vector<Mesh> &object, std::vector<OnSceneObject> &on_scene_objects, float init_x, float init_y, float init_z);
};

extern Scene *gp_scene;
