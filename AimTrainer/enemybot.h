#pragma once
#include "constantbuffer.h"
#include "sharedstruct.h"
#include <vector>

class EnemyBot {
public:
    /// <summary>
    /// コンストラクタでEnemyBotの初期位置を定義
    /// </summary>
    /// <param name="x">botの初期座標x</param>
    /// <param name="y">botの初期座標y</param>
    /// <param name="z">botの初期座標z</param>
    EnemyBot(float x, float y, float z);
    /// <summary>
/// EnemyBotをX方向に往復させる関数
/// </summary>
/// <param name="offset_x">移動速度</param>
/// <param name="range">往復範囲</param>
/// <param name="transform">定数バッファ</param>
/// <param name="boundingbox">botのバウンディングボックス</param>
    void RoundTripX(float offset_x, float range, Transform *transform, BoundingBox &boundingbox);

private:
    const char *mp_mode = "+";
    float m_current_x = NULL;
    float m_current_y = NULL;
    float m_current_z = NULL;
};
