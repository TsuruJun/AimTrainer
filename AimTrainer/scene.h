#pragma once

class Scene {
public:
    bool Init(); // 初期化

    void Update(); // 更新処理
    void Draw(); // 描画処理
};

extern Scene *gp_scene;
