#include "app.h"
#include "engine.h"

HINSTANCE g_hinst;
HWND  g_hwnd = NULL;

// ウィンドウの生成関数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // ウィンドウ終了時の処理
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    // 通常の処理
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void InitWindow(const TCHAR *appname) {
    g_hinst = GetModuleHandle(nullptr);
    if (g_hinst == nullptr) {
        return;
    }

    // ウィンドウの設定
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

    // ウィンドウクラスの登録
    RegisterClassEx(&wndclass);

    // ウィンドウサイズの設定
    RECT rect = {};
    rect.right = static_cast<LONG>(g_window_width);
    rect.bottom = static_cast<LONG>(g_window_height);

    // ウィンドウサイズを調整
    auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRect(&rect, style, false);

    // ウィンドウの生成
    g_hwnd = CreateWindowEx(0, appname, appname, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, g_hinst, nullptr);

    // ウィンドウを表示
    ShowWindow(g_hwnd, SW_SHOWNORMAL);

    // ウィンドウにフォーカスする
    SetFocus(g_hwnd);
}

void MainLoop() {
    MSG msg = {};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE == true)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // 後で描画処理を行う
        }
    }
}

void StartApp(const TCHAR *appname) {
    // ウィンドウ生成
    InitWindow(appname);

    // 描画エンジンの初期化を行う
    gp_engine = new Engine();
    if (!gp_engine->Init(g_hwnd, g_window_width, g_window_height)) {
        return;
    }

    // 後でここで3Dモデルの初期化を行う

    // メイン処理ループ
    MainLoop();
}