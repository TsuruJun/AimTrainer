#include "enemybot.h"
#include "fbxloader.h"
#include "engine.h"
using namespace std;

/// <summary>
/// �R���X�g���N�^��EnemyBot�̏����ʒu���`
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
