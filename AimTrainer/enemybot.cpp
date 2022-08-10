#include "enemybot.h"
#include "engine.h"
#include "fbxloader.h"
using namespace std;

/// <summary>
/// コンストラクタでEnemyBotの初期位置を定義
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
EnemyBot::EnemyBot(float x, float y, float z) {
    m_current_x = x;
    m_current_y = y;
    m_current_z = z;
}

/// <summary>
/// EnemyBotをX方向に往復させる関数
/// </summary>
/// <param name="offset_x">移動速度</param>
/// <param name="range">往復範囲</param>
/// <param name="transform">定数バッファ</param>
void EnemyBot::RoundTripX(float offset_x, float range, Transform *transform) {
    if (m_current_x > range) { // 右に行き過ぎたら進路を左に変える
        mp_mode = "-";
    } else if (m_current_x < -range) { // 左に行き過ぎたら進路を右に変える
        mp_mode = "+";
    }

    // 移動
    if (mp_mode == "+") {
        m_current_x += offset_x;
    } else if (mp_mode == "-") {
        m_current_x -= offset_x;
    }

    transform->world = DirectX::XMMatrixTranslation(m_current_x, m_current_y, m_current_z); // X方向に移動
}
