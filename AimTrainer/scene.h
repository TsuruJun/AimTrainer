#pragma once

class Scene {
public:
    bool Init(); // ������

    void Update(); // �X�V����
    void Draw(); // �`�揈��
};

extern Scene *gp_scene;
