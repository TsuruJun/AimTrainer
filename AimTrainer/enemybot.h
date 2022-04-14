#pragma once
#include <vector>
#include "sharedstruct.h"
#include "constantbuffer.h"

class EnemyBot {
public:
    EnemyBot(float x, float y, float z);
    void RoundTripX(float offset_x, float range, Transform *transform);

private:
    const char *mp_mode = "+";
    float m_current_x = NULL;
    float m_current_y = NULL;
    float m_current_z = NULL;
};