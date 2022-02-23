#include "engine.h"
#include <stdio.h>
#include <Windows.h>

Engine *gp_engine;

bool Engine::Init(HWND hwnd, UINT window_width, UINT window_height) {
    m_framebuffer_width = window_width;
    m_framebuffer_height = window_height;
    m_hwnd = hwnd;

    printf("•`‰æƒGƒ“ƒWƒ“‚Ì‰Šú‰»‚É¬Œ÷\n");
    return true;
}
