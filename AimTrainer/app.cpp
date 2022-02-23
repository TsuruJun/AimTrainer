#include "app.h"
#include "engine.h"

HINSTANCE g_hinst;
HWND  g_hwnd = NULL;

// �E�B���h�E�̐����֐�
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // �E�B���h�E�I�����̏���
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    // �ʏ�̏���
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void InitWindow(const TCHAR *appname) {
    g_hinst = GetModuleHandle(nullptr);
    if (g_hinst == nullptr) {
        return;
    }

    // �E�B���h�E�̐ݒ�
    WNDCLASSEX wndclass = {};
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WindowProc;
    wndclass.hIcon = LoadIcon(g_hinst, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(g_hinst, IDC_ARROW);
    wndclass.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
    wndclass.lpszMenuName = nullptr;
    wndclass.lpszClassName = appname;
    wndclass.hIconSm = LoadIcon(g_hinst, IDI_APPLICATION);

    // �E�B���h�E�N���X�̓o�^
    RegisterClassEx(&wndclass);

    // �E�B���h�E�T�C�Y�̐ݒ�
    RECT rect = {};
    rect.right = static_cast<LONG>(g_window_width);
    rect.bottom = static_cast<LONG>(g_window_height);

    // �E�B���h�E�T�C�Y�𒲐�
    auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRect(&rect, style, false);

    // �E�B���h�E�̐���
    g_hwnd = CreateWindowEx(0, appname, appname, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, g_hinst, nullptr);

    // �E�B���h�E��\��
    ShowWindow(g_hwnd, SW_SHOWNORMAL);

    // �E�B���h�E�Ƀt�H�[�J�X����
    SetFocus(g_hwnd);
}

void MainLoop() {
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == true)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // ��ŕ`�揈�����s��
        }
    }
}

void StartApp(const TCHAR *appname) {
    // �E�B���h�E����
    InitWindow(appname);

    // �`��G���W���̏��������s��
    gp_engine = new Engine();
    if (!gp_engine->Init(g_hwnd, g_window_width, g_window_height)) {
        return;
    }

    // ��ł�����3D���f���̏��������s��

    // ���C���������[�v
    MainLoop();
}