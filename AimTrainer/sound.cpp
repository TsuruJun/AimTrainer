#include "sound.h"
#include "engine.h"
#include <stdio.h>

/// <summary>
/// DirectSoundをコンストラクタで初期化
/// </summary>
Sound::Sound() {
    // DirectSoundの生成
    auto hresult = DirectSoundCreate8(NULL, &m_sound_interface, NULL);
    if (FAILED(hresult)) {
        printf("DirectSoundの生成に失敗");
        return;
    }

    // 強調レベルの設定
    hresult = m_sound_interface->SetCooperativeLevel(gp_engine->GetHwnd(), DSSCL_NORMAL);
    if (FAILED(hresult)) {
        printf("強調レベルの設定に失敗");
        return;
    }
}
