#pragma once
#include <vector>
#include "sharedstruct.h"
#include "constantbuffer.h"

class EnemyBot {
public:
    bool EnemyLoad(const char *p_file_path, std::vector<Mesh> &meshes);
    void RoundTripX(float offset_x, float range, Transform *transform);

    void InitXYZ(float x, float y, float z);

private:
    const char *mp_mode = "+";
    float m_current_x = NULL;
    float m_current_y = NULL;
    float m_current_z = NULL;
};