#pragma once

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include <tchar.h>

#include "SoftwareRasterizer\RenderingContext.h"
#include "SoftwareRasterizer\Utility.h"

#include "MeshData.h"

#include <cstdint>
#include <algorithm>

namespace Test
{
    using namespace SoftwareRasterizer;

    const uint16_t* kDefaultIndices = []
        {
            static uint16_t indices[0xffff + 1];// 0,1,2,3,..,65535
            for (uint32_t i = 0; i <= 0xffff; i++)
            {
                indices[i] = (uint16_t)i;
            }
            return indices;
        } ();


    struct UniformBlock
    {
        Matrix4x4 projectionMatrix;
        Matrix4x4 viewMatrix;
        Matrix4x4 modelMatrix;
        const Sampler2D* meshTexture;
    };

    void LineVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->attributes[0];
        const Vector4& color = input->attributes[1];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyings[0] = color;
        output->varyingNum = 1;
    }

    void LinePixelShaderMain(const FragmentShaderInput* input, FragmentShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector4& color = input->varyings[0];

        output->fragColor = color;
    }

    void MeshVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->attributes[0];
        const Vector4& uv = input->attributes[1];
        const Vector4& normal = input->attributes[2];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyings[0] = uv;
        output->varyings[1] = modelMatrix * normal;
        output->varyingNum = 2;
    }

    void MeshPixelShaderMain(const FragmentShaderInput* input, FragmentShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector2 uv = input->varyings[0].getXY();
        const Vector4& normal = input->varyings[1];

        output->fragColor = TextureUtility::texture2D(uniformBlock->meshTexture, uv);
    }

    class Application
    {

        struct TestCamera
        {
            float fovY = 60.0f * (3.14159265359f / 180.0f); // 上下の視野角

            float nearZ = 0.3f; // ニアクリップ面までの距離
            float farZ = 1000.0f; // ファークリップ面までの距離

            float focusPositionX = 0.0f;// 注視点
            float focusPositionY = 0.5f;
            float focusPositionZ = 0.0f;

            float angleX = 0.0f; // X軸回転角度
            float angleY = 0.0f; // Y軸回転角度
            float zoom = 3.0f;
        };

    public:

        void RenderScene(RenderingContext* _renderingContext, const TestCamera* camera)
        {
            UniformBlock uniformBlock = {};
            uniformBlock.modelMatrix = Matrix4x4::kIdentity;
            _renderingContext->setUniformBlock(&uniformBlock);

            _renderingContext->clearRenderTarget();

            // 射影行列とビュー行列とを作成
            {
                int screenWidth = _renderingContext->getViewportWidth();
                int screenHeight = _renderingContext->getViewportHeight();
                float aspect = (float)screenWidth / (float)screenHeight;
                uniformBlock.projectionMatrix = MatrixUtility::CreatePerspective(camera->fovY, aspect, camera->nearZ, camera->farZ);

                Matrix4x4 rotationX = MatrixUtility::CreateRotationX(camera->angleX);
                Matrix4x4 rotationY = MatrixUtility::CreateRotationY(camera->angleY);
                Vector4 offset(0.0f, 0.0f, camera->zoom, 1.0f);
                offset = (rotationY * rotationX) * offset;
                Vector3 eye = Vector3(camera->focusPositionX, camera->focusPositionY, camera->focusPositionZ) + Vector3(offset.x, offset.y, offset.z);
                Vector3 center(camera->focusPositionX, camera->focusPositionY, camera->focusPositionZ);
                Vector3 up(0.0f, 1.0f, 0.0f);
                uniformBlock.viewMatrix = MatrixUtility::CreateLookAt(eye, center, up);
            }

            // グリッドを描画
            {
                const int gridSize = 9;
                float harfGridSize = gridSize / 2.0f;
                Vector3 gridPositions[2 * 2 * gridSize];
                Vector4 gridColors[2 * 2 * gridSize];
                for (int i = 0; i < gridSize; i++)
                {
                    float a = 1.0f * (i - (gridSize / 2));
                    gridPositions[2 * i] = { -harfGridSize, 0.0f,  a };
                    gridPositions[2 * i + 1] = { harfGridSize, 0.0f, a };
                    gridPositions[2 * gridSize + 2 * i] = { a, 0.0f, -harfGridSize };
                    gridPositions[2 * gridSize + 2 * i + 1] = { a, 0.0f, harfGridSize };
                }
                for (int i = 0; i < (2 * 2 * gridSize); i++)
                {
                    gridColors[i] = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
                }

                _renderingContext->enableVertexAttribute(0);
                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), gridPositions);
                _renderingContext->enableVertexAttribute(1);
                _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), gridColors);
                _renderingContext->setIndexBuffer(kDefaultIndices, 2 * 2 * gridSize);
                _renderingContext->setVertexShaderProgram(LineVertexShaderMain);
                _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

                _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

                _renderingContext->disableVertexAttribute(0);
                _renderingContext->disableVertexAttribute(1);
            }

            // 座標軸を描画
            {
                const Vector3 xAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } };
                const Vector4 xAxisColors[2] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } };
                const Vector3 yAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } };
                const Vector4 yAxisColors[2] = { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
                const Vector3 zAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } };
                const Vector4 zAxisColors[2] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

                _renderingContext->enableVertexAttribute(0);
                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), xAxisPositions);
                _renderingContext->enableVertexAttribute(1);
                _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), xAxisColors);
                _renderingContext->setIndexBuffer(kDefaultIndices, 2);
                _renderingContext->setVertexShaderProgram(LineVertexShaderMain);
                _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);

                _renderingContext->setDepthFunc(ComparisonType::kLessEqual);
                _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), yAxisPositions);
                _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), yAxisColors);
                _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), zAxisPositions);
                _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), zAxisColors);
                _renderingContext->drawIndexed(PrimitiveTopologyType::kLineList);

                _renderingContext->disableVertexAttribute(0);
                _renderingContext->disableVertexAttribute(1);
                _renderingContext->setDepthFunc(ComparisonType::kDefault);

            }

            // 色付き三角形を描画（頂点色は赤緑青の順、反時計周り）
            {
                const Vector3 polygonPositions[3] = { { -1.0f, 0.0f, 0.0f }, { -1.0f, 2.0f,  0.0f }, { -3.0f,  0.0f,  0.0f } };
                const Vector4 polygonColors[3] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };

                _renderingContext->setIndexBuffer(kDefaultIndices, 3);
                _renderingContext->enableVertexAttribute(0);
                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(Vector3), polygonPositions);
                _renderingContext->enableVertexAttribute(1);
                _renderingContext->setVertexAttribute(1, SemanticsType::kColor, 4, ComponentType::kFloat, sizeof(Vector4), polygonColors);
                _renderingContext->setVertexShaderProgram(LineVertexShaderMain);// 流用
                _renderingContext->setFragmentShaderProgram(LinePixelShaderMain);// 流用

                _renderingContext->setFrontFaceType(FrontFaceType::kCounterClockwise);
                _renderingContext->setCullFaceType(CullFaceType::kBack);

                _renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

                _renderingContext->disableVertexAttribute(0);
                _renderingContext->disableVertexAttribute(1);
                _renderingContext->setFrontFaceType(FrontFaceType::kDefault);
                _renderingContext->setCullFaceType(CullFaceType::kDefault);
            }

            // モデル（１メッシュ）を描画
            {
                uniformBlock.modelMatrix = MatrixUtility::CreateRotationX(90.0f * 3.14f / 180.0f);

                Texture2D texture = {};
                Sampler2D sampler2D = { &texture };
                texture.addr = kTexture;
                texture.width = 256;
                texture.height = 256;
                texture.widthBytes = 256;
                uniformBlock.meshTexture = &sampler2D;

                _renderingContext->setIndexBuffer(kMeshTriangles, kMeshTrianglesLength);
                _renderingContext->enableVertexAttribute(0);
                _renderingContext->setVertexAttribute(0, SemanticsType::kPosition, 3, ComponentType::kFloat, sizeof(float) * 3, kMeshVertices);
                _renderingContext->enableVertexAttribute(1);
                _renderingContext->setVertexAttribute(1, SemanticsType::kTexCoord, 2, ComponentType::kFloat, sizeof(float) * 2, kMeshUvs);
                _renderingContext->enableVertexAttribute(2);
                _renderingContext->setVertexAttribute(2, SemanticsType::kNormal, 3, ComponentType::kFloat, sizeof(float) * 3, kMeshNormals);
                _renderingContext->setVertexShaderProgram(MeshVertexShaderMain);
                _renderingContext->setFragmentShaderProgram(MeshPixelShaderMain);

                _renderingContext->setFrontFaceType(FrontFaceType::kCounterClockwise);
                _renderingContext->setCullFaceType(CullFaceType::kBack);

                _renderingContext->drawIndexed(PrimitiveTopologyType::kTriangleList);

                _renderingContext->disableVertexAttribute(0);
                _renderingContext->disableVertexAttribute(1);
                _renderingContext->disableVertexAttribute(2);
                _renderingContext->setFrontFaceType(FrontFaceType::kDefault);
                _renderingContext->setCullFaceType(CullFaceType::kDefault);

                uniformBlock.modelMatrix = Matrix4x4::kIdentity;
                uniformBlock.meshTexture = nullptr;
            }
        }

        //private:

        POINT _lastMousePos;
        bool _isDragging = false;

        TestCamera _camera;

    };

    class MainWindow
    {

    public:

        MainWindow()
        {
        }

        ~MainWindow()
        {
            if (_hwnd != NULL)
            {
                DestroyWindow(_hwnd);
            }
        }

        bool create(HINSTANCE hInstance)
        {
            const TCHAR CLASS_NAME[] = TEXT("SoftwareRasterizer");

            WNDCLASSEX wcex = {};
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = (WNDPROC)WndProc;
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

            return _hwnd != NULL;
        }

        void show(int nShowCmd)
        {
            ShowWindow(_hwnd, nShowCmd);
        }

    private:

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            MainWindow* self;

            if (uMsg == WM_NCCREATE)
            {
                LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
                self = (MainWindow*)(cs->lpCreateParams);
                self->_hwnd = hwnd;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            }
            else
            {
                self = (MainWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            }

            if (self != nullptr)
            {
                return self->wndProc(hwnd, uMsg, wParam, lParam);
            }
            else
            {
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }

        LRESULT wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            switch (uMsg)
            {

            case WM_LBUTTONDOWN:

                _application._lastMousePos.x = LOWORD(lParam);
                _application._lastMousePos.y = HIWORD(lParam);
                _application._isDragging = true;
                SetCapture(hwnd);
                return 0;

            case WM_MOUSEMOVE:
                if (_application._isDragging)
                {
                    POINT currentMousePos;
                    currentMousePos.x = LOWORD(lParam);
                    currentMousePos.y = HIWORD(lParam);

                    int dx = currentMousePos.x - _application._lastMousePos.x;
                    int dy = currentMousePos.y - _application._lastMousePos.y;

                    _application._lastMousePos = currentMousePos;

                    _application._camera.angleX += dy * 0.01f; // Y軸回転
                    _application._camera.angleY += dx * 0.01f; // X軸回転
                    InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
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
                GetClientRect(hwnd, &clientRect);// left と top は常に 0

                int frameWidth = clientRect.right;
                int frameHeight = clientRect.bottom;
                //frameWidth = clientRect.right / 2;
                //frameHeight = clientRect.bottom / 2;

                HDC hWndDC = GetDC(hwnd);
                BITMAPINFOHEADER bih = {};
                bih.biSize = sizeof(BITMAPINFOHEADER);
                bih.biWidth = frameWidth;
                bih.biHeight = frameHeight;
                bih.biPlanes = 1;
                bih.biBitCount = 32;
                bih.biCompression = BI_RGB;
                VOID* pvBits = nullptr;
                _hDibBm = CreateDIBSection(hWndDC, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pvBits, NULL, 0);
                ReleaseDC(hwnd, hWndDC);

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
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;
            }

            case WM_LBUTTONUP:

                _application._isDragging = false;

                ReleaseCapture();
                return 0;

            case WM_MOUSEWHEEL:
            {
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                _application._camera.zoom += zDelta * -0.001f;
                _application._camera.zoom = std::clamp(_application._camera.zoom, 0.001f, FLT_MAX);
                InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
                return 0;
            }

            case WM_KEYDOWN:
                switch (wParam)
                {
                case 'W':
                    _application._camera.focusPositionZ += 0.1f;
                    break;
                case 'S':
                    _application._camera.focusPositionZ -= 0.1f;
                    break;
                case 'A':
                    _application._camera.focusPositionX -= 0.1f;
                    break;
                case 'D':
                    _application._camera.focusPositionX += 0.1f;
                    break;
                case 'Q':
                    _application._camera.focusPositionY += 0.1f;
                    break;
                case 'E':
                    _application._camera.focusPositionY -= 0.1f;
                    break;
                }
                // 再描画を要求
                InvalidateRect(hwnd, NULL, TRUE);
                return 0;

            case WM_PAINT:
            {
                PAINTSTRUCT ps = {};
                BeginPaint(hwnd, &ps);

                // DIBにシーンをレンダリング
                _application.RenderScene(&_renderingContext, &_application._camera);

                // レンダリング結果をウィンドウへ転送
                HDC hdcSrc = CreateCompatibleDC(ps.hdc);
                HGDIOBJ hBmPrev = SelectObject(hdcSrc, _hDibBm);
                BitBlt(ps.hdc, 0, 0, _renderingContext.getWindowWidth(), _renderingContext.getWindowHeight(), hdcSrc, 0, 0, SRCCOPY);
                //StretchBlt(ps.hdc, 0, 0, g_renderingContext.getFrameWidth() * 2, g_renderingContext.getFrameHeight() * 2, hdcSrc, 0, 0, g_renderingContext.getFrameWidth(), g_renderingContext.getFrameHeight(), SRCCOPY);
                SelectObject(hdcSrc, hBmPrev);
                DeleteDC(hdcSrc);

                EndPaint(hwnd, &ps);
                return 0;
            }

            default:
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }

    private:

        HWND _hwnd = NULL;

        HBITMAP _hDibBm = NULL;         // カラーバッファ
        float* _depthBuffer = nullptr;  // 

        RenderingContext _renderingContext;

        Application _application;

    };
}
