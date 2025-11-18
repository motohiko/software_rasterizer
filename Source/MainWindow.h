#pragma once

#include "Test.h"
#include "SoftwareRasterizer\RenderingContext.h"

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"

using namespace Test;
using namespace SoftwareRasterizer;

class MainWindow
{

public:

    MainWindow();
    ~MainWindow();

    bool create(HINSTANCE hInstance);

    void show(int nShowCmd);

private:

    static LRESULT CALLBACK RedirectWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

    HWND _hwnd = NULL;

    HBITMAP _hDibBm = NULL;         // カラーバッファ
    float* _depthBuffer = nullptr;  // 

    RenderingContext _renderingContext;

    int _lastMousePosX = 0;
    int _lastMousePosY = 0;
    bool _isDragging = false;

    TestCamera _camera;
    ModelViewer _scene;

};
