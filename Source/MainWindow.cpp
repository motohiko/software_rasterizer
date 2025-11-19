#include "MainWindow.h" 

#include <tchar.h>
#include <algorithm>
#include <cmath>
#include <stdio.h>

void PrintDebugLog(const TCHAR* format, ...)
{
    TCHAR buffer[512];
    va_list args;
    va_start(args, format);
    _vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), format, args);
    va_end(args);
    OutputDebugString(buffer);
}

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
    if (_hwnd != NULL)
    {
        DestroyWindow(_hwnd);
    }
}

bool MainWindow::create(HINSTANCE hInstance)
{
    const TCHAR CLASS_NAME[] = TEXT("SoftwareRasterizer");

    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    CreateWindowEx(
        0,
        CLASS_NAME,
        TEXT("SoftwareRasterizer"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        (LPVOID)this
    );

    return (_hwnd != NULL);
}

void MainWindow::show(int nShowCmd)
{
    ShowWindow(_hwnd, nShowCmd);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* self = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (self == nullptr)
    {
        if (uMsg == WM_NCCREATE)
        {
            LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
            self = (MainWindow*)(cs->lpCreateParams);
            self->_hwnd = hwnd;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
        }
        else
        {   
            // 想定外のメッセージ
            if (uMsg == WM_GETMINMAXINFO)
            {
                // WM_NCCREATE 前の WM_GETMINMAXINFO
                OutputDebugString(TEXT("unexpected message. (WM_GETMINMAXINFO)\n"));
            }
            else
            {
                OutputDebugString(TEXT("unexpected message.\n"));
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    LRESULT result = self->handleMessage(uMsg, wParam, lParam);

    if (uMsg == WM_NCDESTROY)
    {
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        self->_hwnd = NULL;
    }

    return result;
}

LRESULT MainWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_LBUTTONDOWN:

        _lastMousePosX = LOWORD(lParam);
        _lastMousePosY = HIWORD(lParam);
        _isDragging = true;
        SetCapture(_hwnd);
        return 0;

    case WM_MOUSEMOVE:
        if (_isDragging)
        {
            POINT currentMousePos;
            currentMousePos.x = LOWORD(lParam);
            currentMousePos.y = HIWORD(lParam);

            int dx = currentMousePos.x - _lastMousePosX;
            int dy = currentMousePos.y - _lastMousePosY;

            _lastMousePosX = currentMousePos.x;
            _lastMousePosY = currentMousePos.y;

            _camera.angleX += dy * 0.01f;
            _camera.angleY += dx * 0.01f;

            // 再描画を要求
            InvalidateRect(_hwnd, NULL, TRUE);
        }
        return 0;

    case WM_DESTROY:

        if (NULL != _hDibBm)
        {
            DeleteObject(_hDibBm);
            _hDibBm = NULL;
        }

        if (nullptr != _depthBuffer)
        {
            free(_depthBuffer);
            _depthBuffer = nullptr;
        }

        PostQuitMessage(0);

        return 0;

    case WM_SIZE:
    {
        _renderingContext.setWindowSize(0, 0);
        _renderingContext.setRenderTargetColorBuffer(nullptr, 0, 0, 0);
        _renderingContext.setRenderTargetDepthBuffer(nullptr, 0, 0, 0);
        _renderingContext.setViewport(0, 0, 0, 0);

        if (NULL != _hDibBm)
        {
            DeleteObject(_hDibBm);
            _hDibBm = NULL;
        }

        if (nullptr != _depthBuffer)
        {
            free(_depthBuffer);
            _depthBuffer = nullptr;
        }

        RECT clientRect = {};
        GetClientRect(_hwnd, &clientRect);// left と top は常に 0

        int frameWidth = clientRect.right;
        int frameHeight = clientRect.bottom;
        //frameWidth = clientRect.right / 2;
        //frameHeight = clientRect.bottom / 2;

        HDC hWndDC = GetDC(_hwnd);
        BITMAPINFOHEADER bih = {};
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = frameWidth;
        bih.biHeight = frameHeight;
        bih.biPlanes = 1;
        bih.biBitCount = 32;
        bih.biCompression = BI_RGB;
        VOID* pvBits = nullptr;
        _hDibBm = CreateDIBSection(hWndDC, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pvBits, NULL, 0);
        ReleaseDC(_hwnd, hWndDC);

        DIBSECTION dibSection = {};
        if (NULL != _hDibBm)
        {
            GetObject(_hDibBm, sizeof(DIBSECTION), &dibSection);
        }

        _depthBuffer = (float*)malloc(sizeof(float) * dibSection.dsBm.bmWidth * dibSection.dsBm.bmHeight);

        _renderingContext.setWindowSize(frameWidth, frameHeight);
        _renderingContext.setRenderTargetColorBuffer(dibSection.dsBm.bmBits, frameWidth, frameHeight, dibSection.dsBm.bmWidthBytes);
        _renderingContext.setRenderTargetDepthBuffer(_depthBuffer, frameWidth, frameHeight, sizeof(float) * dibSection.dsBm.bmWidth);
        _renderingContext.setViewport(0, 0, frameWidth, frameHeight);

        // 再描画を要求
        InvalidateRect(_hwnd, NULL, TRUE);
        return 0;
    }

    case WM_LBUTTONUP:

        _isDragging = false;

        ReleaseCapture();
        return 0;

    case WM_MOUSEWHEEL:
    {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        _camera.zoom += zDelta * -0.001f;
        _camera.zoom = std::clamp(_camera.zoom, 0.001f, FLT_MAX);
        // 再描画を要求
        InvalidateRect(_hwnd, NULL, TRUE);
        return 0;
    }

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'W':
            _camera.focusPositionZ += 0.1f;
            break;
        case 'S':
            _camera.focusPositionZ -= 0.1f;
            break;
        case 'A':
            _camera.focusPositionX -= 0.1f;
            break;
        case 'D':
            _camera.focusPositionX += 0.1f;
            break;
        case 'Q':
            _camera.focusPositionY += 0.1f;
            break;
        case 'E':
            _camera.focusPositionY -= 0.1f;
            break;
        }
        // 再描画を要求
        InvalidateRect(_hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps = {};
        BeginPaint(_hwnd, &ps);

        // DIBにレンダリング
        _scene.RenderScene(&_renderingContext, &_camera);

        // レンダリング結果をウィンドウへ転送
        HDC hdcSrc = CreateCompatibleDC(ps.hdc);
        HGDIOBJ hBmPrev = SelectObject(hdcSrc, _hDibBm);
        BitBlt(ps.hdc, 0, 0, _renderingContext.getWindowWidth(), _renderingContext.getWindowHeight(), hdcSrc, 0, 0, SRCCOPY);
        //StretchBlt(ps.hdc, 0, 0, g_renderingContext.getFrameWidth() * 2, g_renderingContext.getFrameHeight() * 2, hdcSrc, 0, 0, g_renderingContext.getFrameWidth(), g_renderingContext.getFrameHeight(), SRCCOPY);
        SelectObject(hdcSrc, hBmPrev);
        DeleteDC(hdcSrc);

        EndPaint(_hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProc(_hwnd, uMsg, wParam, lParam);
    }

}
