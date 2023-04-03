#include "dinputhelper.h"
#include "engine.h"
#include <stdio.h>

DInputHelper::DInputHelper() {
    // DInputを初期化
    // インターフェースを作成
    HRESULT hresult = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_direct_input_interface, NULL);

    if (FAILED(hresult)) {
        printf("DirectInputのインターフェースの生成に失敗");
        return;
    }

    // デバイスを作成
    hresult = m_direct_input_interface->CreateDevice(GUID_SysMouse, &m_direct_input_device, NULL);

    if (FAILED(hresult)) {
        printf("DirectInputのデバイスの生成に失敗");
        return;
    }

    // フォーマットの設定
    hresult = m_direct_input_device->SetDataFormat(&c_dfDIMouse);
    if (FAILED(hresult)) {
        printf("DirectInputのフォーマットの設定に失敗");
        return;
    }

    // 強調モードの設定
    hresult = m_direct_input_device->SetCooperativeLevel(gp_engine->GetHwnd(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    if (FAILED(hresult)) {
        printf("DirectInputの強調モードの設定に失敗");
        return;
    }

    // 制御開始
    hresult = m_direct_input_device->Acquire();
    if (FAILED(hresult)) {
        printf("制御開始に失敗");
        return;
    }

    // ポーリング開始
    hresult = m_direct_input_device->Poll();
    if (FAILED(hresult)) {
        printf("ポーリング開始に失敗");
        return;
    }
}

DInputHelper::~DInputHelper() {
    // DirectInputの終了時処理
    // デバイス制御停止
    m_direct_input_device->Unacquire();

    // デバイスの解放
    m_direct_input_device->Release();

    // インターフェースの解放
    m_direct_input_interface->Release();
}

bool DInputHelper::GetMouseState() {
    HRESULT hresult = m_direct_input_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_current_mouse_state);
    if (FAILED(hresult)) {
        printf("マウスの状態取得に失敗");

        m_direct_input_device->Acquire();
        hresult = m_direct_input_device->GetDeviceState(sizeof(DIMOUSESTATE), &m_current_mouse_state);

        return false;
    }
    return true;
}

bool DInputHelper::isLeftClick() {
    GetMouseState();
    if (m_current_mouse_state.rgbButtons[0] & (0x80)) {
        return true;
    } else {
        return false;
    }
}


