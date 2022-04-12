#include "enemybot.h"
#include "fbxloader.h"
#include "engine.h"
using namespace std;

/// <summary>
/// EnemyBot�����[�h����֐�
/// </summary>
/// <param name="p_file_path">EnemyBot�̃t�@�C���p�X</param>
/// <param name="meshes">�ǂݍ��񂾃��f����n�����߂̎󂯎M</param>
/// <returns>true: ���� false: ���s</returns>
bool EnemyBot::EnemyLoad(const char *p_file_path, vector<Mesh> &meshes) {
    FbxLoader loader;
    if (!loader.FbxLoad(p_file_path)) {
        printf("EnemyBot�̓ǂݍ��݂Ɏ��s");
        return false;
    }
    meshes = loader.GetMeshes();
    loader.ClearMeshes();

    return true;
}

/// <summary>
/// EnemyBot��X�����ɉ���������֐�
/// </summary>
/// <param name="offset_x">�ړ����x</param>
/// <param name="range">�����͈�</param>
/// <param name="transform">�萔�o�b�t�@</param>
void EnemyBot::RoundTripX(float offset_x, float range, Transform *transform) {
    if (m_current_x > range) { // �E�ɍs���߂�����i�H�����ɕς���
        mp_mode = "-";
    } else if (m_current_x < -range) { // ���ɍs���߂�����i�H���E�ɕς���
        mp_mode = "+";
    }

    // �ړ�
    if (mp_mode == "+") {
        m_current_x += offset_x;
    } else if (mp_mode == "-") {
        m_current_x -= offset_x;
    }

    transform->world = DirectX::XMMatrixTranslation(m_current_x, m_current_y, m_current_z); // X�����Ɉړ�
}

/// <summary>
/// EnemyBot�̍��W������������Ă��Ȃ��Ƃ��A����������
/// </summary>
/// <param name="x">x���W</param>
/// <param name="y">y���W</param>
/// <param name="z">z���W</param>
void EnemyBot::InitXYZ(float x, float y, float z) {
    if (m_current_x == NULL && m_current_y == NULL && m_current_z == NULL) {
        m_current_x = x;
        m_current_y = y;
        m_current_z = z;
    }
}
