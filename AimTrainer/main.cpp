#include "app.h"

int wmain(int argc, wchar_t **argv, wchar_t **envp) {
    // コンソール出力時に文字コードをutf-8にする
    const int sav = GetConsoleOutputCP();
    SetConsoleOutputCP(65001);

    StartApp(TEXT("AimTrainer"));

    // 終わったら文字コードを戻す
    SetConsoleOutputCP(sav);
    return 0;
}
