#pragma once

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"

#include "ModelViewer.h"
#include "SoftwareRasterizer\RenderingContext.h"

class MainWindow
{

public:

    MainWindow();
    ~MainWindow();

    bool create(HINSTANCE hInstance);

    void show(int nShowCmd);

private:

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void createFrameBuffer();
    void deleteFrameBuffer();

private:

    HWND _hwnd = NULL;
    HBITMAP _hDib = NULL;
    void* _depthBuffer = nullptr;

    int _lastMousePosX = 0;
    int _lastMousePosY = 0;
    bool _isLButtonDragging = false;

    SoftwareRasterizer::RenderingContext _renderingContext;
    Test::ModelViewer _modelViewer;

};
