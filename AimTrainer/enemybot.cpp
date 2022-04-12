#include "enemybot.h"
#include "fbxloader.h"
#include "engine.h"
using namespace std;

/// <summary>
/// EnemyBotをロードする関数
/// </summary>
/// <param name="p_file_path">EnemyBotのファイルパス</param>
/// <param name="meshes">読み込んだモデルを渡すための受け皿</param>
/// <returns>true: 成功 false: 失敗</returns>
bool EnemyBot::EnemyLoad(const char *p_file_path, vector<Mesh> &meshes) {
    FbxLoader loader;
    if (!loader.FbxLoad(p_file_path)) {
        printf("EnemyBotの読み込みに失敗");
        return false;
    }
    meshes = loader.GetMeshes();
    loader.ClearMeshes();

    return true;
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

/// <summary>
/// EnemyBotの座標が初期化されていないとき、初期化する
/// </summary>
/// <param name="x">x座標</param>
/// <param name="y">y座標</param>
/// <param name="z">z座標</param>
void EnemyBot::InitXYZ(float x, float y, float z) {
    if (m_current_x == NULL && m_current_y == NULL && m_current_z == NULL) {
        m_current_x = x;
        m_current_y = y;
        m_current_z = z;
    }
}
