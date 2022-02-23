#include "engine.h"
#include <stdio.h>
#include <Windows.h>

Engine *gp_engine;

bool Engine::Init(HWND hwnd, UINT window_width, UINT window_height) {
    m_framebuffer_width = window_width;
    m_framebuffer_height = window_height;
    m_hwnd = hwnd;

    printf("描画エンジンの初期化に成功\n");
    return true;
}
