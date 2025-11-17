
#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include <tchar.h>

#include "MainWindow.h" 
#include <stdio.h>
#include <memory.h>

namespace Test
{
    void PrintDebugLog(const TCHAR* format, ...)
    {
        TCHAR buffer[512];
        va_list args;
        va_start(args, format);
        _vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), format, args);
        va_end(args);
        OutputDebugString(buffer);
    }
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    auto mainWindow = std::make_unique<Test::MainWindow>();

    if (!mainWindow->create(hInstance))
    {
        return 0;
    }

    mainWindow->show(nShowCmd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
