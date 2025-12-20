#include "MainWindow.h" 

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
    if (NULL != _hwnd)
    {
        DestroyWindow(_hwnd);
    }
}

bool MainWindow::create(HINSTANCE hInstance)
{
    const TCHAR CLASS_NAME[] = TEXT("SoftwareRasterizer");

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)NULL;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);
    RegisterClassExW(&wcex);

    CreateWindowExW(
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

    return (NULL != _hwnd);
}

void MainWindow::show(int nShowCmd)
{
    ShowWindow(_hwnd, nShowCmd);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* self;

    switch (uMsg)
    {

    case WM_NCCREATE:
        {
            LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;

            self = (MainWindow*)(lpCreateStruct->lpCreateParams);
            self->_hwnd = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
    
            return self->handleMessage(uMsg, wParam, lParam);
        }

    case WM_NCDESTROY:
        {
            self = (MainWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

            LRESULT result = self->handleMessage(uMsg, wParam, lParam);

            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            self->_hwnd = NULL;

            return result;
        }

    default:
        {
            self = (MainWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (nullptr == self)
            {
                // note. WM_NCCREATE の前に WM_GETMINMAXINFO が一度だけ送られてくる、それ以外は想定外
                OutputDebugStringW(TEXT("GWLP_USERDATA is null.\n"));
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            }

            return self->handleMessage(uMsg, wParam, lParam);
        }

    }
}

LRESULT MainWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_DESTROY:
        {
            deleteFrameBuffer();

            PostQuitMessage(0);
            return 0;
        }

    case WM_SIZE:
        {
            UINT state = (UINT)wParam;
            int cx = (short)LOWORD(lParam);
            int cy = (short)HIWORD(lParam);

            deleteFrameBuffer();
            createFrameBuffer();

            InvalidateRect(_hwnd, NULL, TRUE);
            return 0;
        }

    case WM_LBUTTONDOWN:
        {
            BOOL fDoubleClick = FALSE;
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            UINT keyFlags = (UINT)wParam;

            _isLButtonDragging = true;
            _lastMousePosX = x;
            _lastMousePosY = y;

            SetCapture(_hwnd);
            return 0;
        }

    case WM_LBUTTONUP:
        {
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            UINT keyFlags = (UINT)wParam;

            _isLButtonDragging = false;

            ReleaseCapture();
            return 0;
        }

    case WM_MOUSEMOVE:
        {
            int x = (short)LOWORD(lParam);
            int y = (short)HIWORD(lParam);
            UINT keyFlags = (UINT)wParam;

            if (_isLButtonDragging)
            {
                int xDelta = x - _lastMousePosX;
                int yDelta = y - _lastMousePosY;

                _modelViewer.onLButtonDrag(xDelta, yDelta);

                InvalidateRect(_hwnd, NULL, TRUE);
            }

            _lastMousePosX = x;
            _lastMousePosY = y;

            return 0;
        }

    case WM_MOUSEWHEEL:
        {
            int xPos = (short)LOWORD(lParam);
            int yPos = (short)HIWORD(lParam);
            //int zDelta = (short)HIWORD(wParam);
            UINT fwKeys = (UINT)(short)LOWORD(wParam);

            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

            _modelViewer.onMouseWweel(zDelta);

            InvalidateRect(_hwnd, NULL, TRUE);
            return 0;
        }

    case WM_KEYDOWN:
        {
            UINT vk = (UINT)wParam;
            BOOL fDown = TRUE;
            int cRepeat = (short)LOWORD(lParam);
            UINT flags = (UINT)HIWORD(lParam);

            _modelViewer.onKeyDown(vk);

            InvalidateRect(_hwnd, NULL, TRUE);
            return 0;
        }

    case WM_PAINT:
        {
            PAINTSTRUCT ps = {};
            BeginPaint(_hwnd, &ps);

            // DIBにシーンを描画
            _modelViewer.onPaint(&_renderingContext);

            HDC hdcSrc = CreateCompatibleDC(ps.hdc);
            HGDIOBJ hBmPrev = SelectObject(hdcSrc, _hDib);

            // 描画結果をウィンドウへ転送
            BitBlt(ps.hdc, 0, 0, _renderingContext.getWindowWidth(), _renderingContext.getWindowHeight(), hdcSrc, 0, 0, SRCCOPY);
            //StretchBlt(ps.hdc, 0, 0, g_renderingContext.getFrameWidth() * 2, g_renderingContext.getFrameHeight() * 2, hdcSrc, 0, 0, g_renderingContext.getFrameWidth(), g_renderingContext.getFrameHeight(), SRCCOPY);

            SelectObject(hdcSrc, hBmPrev);
            DeleteDC(hdcSrc);

            EndPaint(_hwnd, &ps);
            return 0;
        }

    default:

        return DefWindowProcW(_hwnd, uMsg, wParam, lParam);

    }

}

void MainWindow::createFrameBuffer()
{
    RECT clientRect = {};
    GetClientRect(_hwnd, &clientRect);// left と top は常に 0
    int clientWidth = clientRect.right;
    int clientHeight = clientRect.bottom;
    //clientWidth = clientRect.right / 2;
    //clientHeight = clientRect.bottom / 2;

    HDC hWndDC = GetDC(_hwnd);

    BITMAPINFOHEADER bih = {};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = clientWidth;
    bih.biHeight = clientHeight;
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;
    VOID* pvBits = nullptr;
    _hDib = CreateDIBSection(hWndDC, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pvBits, NULL, 0);

    ReleaseDC(_hwnd, hWndDC);

    DIBSECTION dibSection = {};
    if (NULL != _hDib)
    {
        GetObjectW(_hDib, sizeof(DIBSECTION), &dibSection);
    }

    int depthByteCount = 4;// TODO: rename

    _depthBuffer = std::malloc(depthByteCount * clientWidth * clientHeight);

    _renderingContext.setWindowSize(clientWidth, clientHeight);
    _renderingContext.setRenderTargetColorBuffer(dibSection.dsBm.bmBits, clientWidth, clientHeight, (int)dibSection.dsBm.bmWidthBytes);
    _renderingContext.setRenderTargetDepthBuffer(_depthBuffer, clientWidth, clientHeight, depthByteCount * dibSection.dsBm.bmWidth);
    _renderingContext.setViewport(0, 0, clientWidth, clientHeight);
    _renderingContext.setViewport(50, 50, clientWidth - 100, clientHeight - 100);
}

void MainWindow::deleteFrameBuffer()
{
    _renderingContext.setWindowSize(0, 0);
    _renderingContext.setRenderTargetColorBuffer(nullptr, 0, 0, 0);
    _renderingContext.setRenderTargetDepthBuffer(nullptr, 0, 0, 0);
    _renderingContext.setViewport(0, 0, 0, 0);

    if (NULL != _hDib)
    {
        DeleteObject(_hDib);
        _hDib = NULL;
    }

    if (nullptr != _depthBuffer)
    {
        std::free(_depthBuffer);
        _depthBuffer = nullptr;
    }
}
