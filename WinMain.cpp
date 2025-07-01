
#include <cstdint>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>// abs

#include "PreIncludeWindows.h"
#include <Windows.h>
#include "PostIncludeWindows.h"
#include <tchar.h>

#include "Algorithm.h"
#include "Vector.h"
#include "Matrix.h"
#include "Renderer.h"
#include "Texture.h"
#include "MeshData.h"

namespace MyApp
{
    // 0,1,2,3,..,65535
    const uint16_t* kDefaultIndices = []
        {
            static uint16_t indices[0xffff + 1];
            for (uint32_t i = 0; i <= 0xffff; ++i)
            {
                indices[i] = (uint16_t)i;
            }
            return indices;
        } ();

    HBITMAP g_hDibBm = NULL;// color buffer
    void* g_depthBuffer = nullptr;
    POINT lastMousePos;
    bool isDragging = false;
    float angleX = 0.0f; // X軸回転角度
    float angleY = 0.0f; // Y軸回転角度
    float cameraX = 0.0f; // カメラのX位置
    float cameraY = 0.5f; // カメラのY位置
    float cameraZ = 0.0f; // カメラのZ位置
    float zoom = 2.0f; // ズームレベル

    struct UniformBlock
    {
        Matrix4x4 projectionMatrix;
        Matrix4x4 viewMatrix;
        Matrix4x4 modelMatrix;
        const Texture* meshTexture;
    };

    void printDebugLog(const TCHAR* format, ...)
    {
        TCHAR buffer[512];
        va_list args;
        va_start(args, format);
        _vstprintf_s(buffer, sizeof(buffer) / sizeof(TCHAR), format, args);
        va_end(args);
        OutputDebugString(buffer);
    }

    void LineVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->elements[0];
        const Vector4& color = input->elements[1];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyingVariables[0] = color;
    }

    void LinePixelShaderMain(const PixelShaderInput* input, PixelShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector4& color = input->varyingVariables[0];
        output->fragColor = color;
    }

    void MeshVertexShaderMain(const VertexShaderInput* input, VertexShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Matrix4x4& projectionMatrix = uniformBlock->projectionMatrix;
        const Matrix4x4& viewMatrix = uniformBlock->viewMatrix;
        const Matrix4x4& modelMatrix = uniformBlock->modelMatrix;
        const Vector4& position = input->elements[0];
        const Vector4& uv = input->elements[1];

        output->position = projectionMatrix * ((viewMatrix * modelMatrix) * position);
        output->varyingVariables[0] = uv;
    }

    void MeshPixelShaderMain(const PixelShaderInput* input, PixelShaderOutput* output)
    {
        const UniformBlock* uniformBlock = (const UniformBlock*)input->uniformBlock;
        const Vector2 uv = input->varyingVariables[0].getXY();

        output->fragColor = sampleTexture(uniformBlock->meshTexture, uv);
    }

    void RenderScene(Renderer* renderer)
    {
        int screenWidth = renderer->getViewportWidth();
        int screenHeight = renderer->getViewportHeight();

        UniformBlock uniformBlock = {};

        {
            float fov = 90.0f * (3.14159265359f / 180.0f); // 視野角
            float aspect = (float)screenWidth / (float)screenHeight; // アスペクト比
            float near = 0.1f; // 近クリップ面
            float far = 15.0f; // 遠クリップ面
            Matrix4x4 projectionMatrix = Matrix4x4::createProjection(fov, aspect, near, far);

            Matrix4x4 rotationX = Matrix4x4::createRotationX(angleX);
            Matrix4x4 rotationY = Matrix4x4::createRotationY(angleY);
            Vector4 tmp(0.0f, 0.0f, zoom, 1.0f);
            tmp = (rotationY * rotationX) * tmp;
            Vector3 eye = Vector3(cameraX, cameraY, cameraZ) + Vector3(tmp.x, tmp.y, tmp.z);
            Vector3 center(cameraX, cameraY, cameraZ);
            Vector3 up(0.0f, 1.0f, 0.0f);
            Matrix4x4 viewMatrix = Matrix4x4::lockAt(eye, center, up);

            uniformBlock.projectionMatrix = projectionMatrix;
            uniformBlock.viewMatrix = viewMatrix;
            uniformBlock.modelMatrix = Matrix4x4::kIdentity;
            renderer->setUniformBlock(&uniformBlock);
        }

        // 座標軸の描画
        {
            const Vector3 xAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 1.0f,  0.0f,  0.0f } };
            Vector4 xAxisColors[2] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } };
            const Vector3 yAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  1.0f,  0.0f } };
            Vector4 yAxisColors[2] = { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
            const Vector3 zAxisPositions[2] = { { 0.0f, 0.0f, 0.0f }, { 0.0f,  0.0f,  1.0f } };
            Vector4 zAxisColors[2] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
            renderer->setPrimitiveTopologyType(PrimitiveTopologyType::LineList);
            renderer->setVertexBufferElement(0, xAxisPositions, sizeof(Vector3));
            renderer->setVertexBufferElement(1, xAxisColors, sizeof(Vector4));
            renderer->setVertexBufferElementsCount(2);
            renderer->setIndexBuffer(kDefaultIndices, 2);
            renderer->setVertexShaderProgram(LineVertexShaderMain);
            renderer->setVaryingVariableCount(1);
            renderer->setPixelShaderProgram(LinePixelShaderMain);
            renderer->drawIndexed();

            renderer->setVertexBufferElement(0, yAxisPositions, sizeof(Vector3));
            renderer->setVertexBufferElement(1, yAxisColors, sizeof(Vector4));
            renderer->setVertexBufferElementsCount(2);
            renderer->drawIndexed();

            renderer->setVertexBufferElement(0, zAxisPositions, sizeof(Vector3));
            renderer->setVertexBufferElement(1, zAxisColors, sizeof(Vector4));
            renderer->setVertexBufferElementsCount(2);
            renderer->drawIndexed();
        }

        // グリッドの描画
        {
            const int gridSize = 9;
            float harfGridSize = gridSize / 2.0f;
            Vector3 gridPositions[2 * 2 * gridSize];
            Vector4 gridColors[2 * 2 * gridSize];
            for (int i = 0; i < gridSize; ++i)
            {
                float a = 1.0f * (i - (gridSize / 2));
                gridPositions[2 * i] = { -harfGridSize, 0.0f,  a };
                gridPositions[2 * i + 1] = { harfGridSize, 0.0f, a };
                gridPositions[2 * gridSize + 2 * i] = { a, 0.0f, -harfGridSize };
                gridPositions[2 * gridSize + 2 * i + 1] = { a, 0.0f, harfGridSize };
            }
            for (int i = 0; i < (2 * 2 * gridSize); ++i)
            {
                gridColors[i] = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
            }
            renderer->setPrimitiveTopologyType(PrimitiveTopologyType::LineList);
            renderer->setVertexBufferElement(0, gridPositions, sizeof(Vector3));
            renderer->setVertexBufferElement(1, gridColors, sizeof(Vector4));
            renderer->setVertexBufferElementsCount(2);
            renderer->setIndexBuffer(kDefaultIndices, 2 * 2 * gridSize);
            renderer->setVertexShaderProgram(LineVertexShaderMain);
            renderer->setVaryingVariableCount(1);
            renderer->setPixelShaderProgram(LinePixelShaderMain);
            renderer->drawIndexed();
        }

        // ポリゴンの描画（頂点色は赤緑青の順、反時計周り）
        {
            const Vector3 polygonPositions[3] = { { -1.0f, 0.0f, 0.0f }, { -1.0f, 2.0f,  0.0f }, { -3.0f,  0.0f,  0.0f } };
            Vector4 polygonColors[3] = { { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
            renderer->setPrimitiveTopologyType(PrimitiveTopologyType::TriangleList);
            renderer->setVertexBufferElement(0, polygonPositions, sizeof(Vector3));
            renderer->setVertexBufferElement(1, polygonColors, sizeof(Vector4));
            renderer->setVertexBufferElementsCount(2);
            renderer->setIndexBuffer(kDefaultIndices, 3);
            renderer->setVertexShaderProgram(LineVertexShaderMain);// 流用
            renderer->setVaryingVariableCount(1);
            renderer->setPixelShaderProgram(LinePixelShaderMain);// 流用
            renderer->drawIndexed();
        }


        // メッシュの描画
        {
            uniformBlock.modelMatrix = Matrix4x4::createRotationX(90.0f * 3.14f / 180.0f);

            Texture meshTexture = {};
            meshTexture.addr = kTexture;
            meshTexture.width = 256;
            meshTexture.height = 256;
            uniformBlock.meshTexture = &meshTexture;

            renderer->setVertexBufferElement(0, kMeshVertices, sizeof(Vector3));
            renderer->setVertexBufferElement(1, kMeshUvs, sizeof(Vector2));
            renderer->setVertexBufferElementsCount(2);
            renderer->setIndexBuffer(kMeshTriangles, kMeshTrianglesLength);
            renderer->setVertexShaderProgram(MeshVertexShaderMain);
            renderer->setVaryingVariableCount(1);
            renderer->setPixelShaderProgram(MeshPixelShaderMain);
            renderer->setPrimitiveTopologyType(PrimitiveTopologyType::TriangleList);
            renderer->drawIndexed();
        }
    }

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) 
        {
        case WM_LBUTTONDOWN:
        {
            lastMousePos.x = LOWORD(lParam);
            lastMousePos.y = HIWORD(lParam);
            isDragging = true;
            SetCapture(hwnd);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            if (isDragging)
            {
                POINT currentMousePos = {};
                currentMousePos.x = LOWORD(lParam);
                currentMousePos.y = HIWORD(lParam);

                int dx = currentMousePos.x - lastMousePos.x;
                int dy = currentMousePos.y - lastMousePos.y;

                lastMousePos = currentMousePos;

                angleX += dy * 0.01f; // Y軸回転
                angleY += dx * 0.01f; // X軸回転
                InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
            }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            isDragging = false;
            ReleaseCapture();
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            zoom += zDelta * -0.001f;
            zoom = clamp(zoom, 0.001f, FLT_MAX);
            InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
            return 0;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case 'W':
                cameraZ += 0.1f;
                break;
            case 'S':
                cameraZ -= 0.1f;
                break;
            case 'A':
                cameraX -= 0.1f;
                break;
            case 'D':
                cameraX += 0.1f;
                break;
            case 'Q':
                cameraY += 0.1f;
                break;
            case 'E':
                cameraY -= 0.1f;
                break;
            }
            InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
            return 0;
        }
        case WM_SIZE:
        {
            if (NULL != g_hDibBm)
            {
                DeleteObject(g_hDibBm);
                g_hDibBm = NULL;
            }

            if (nullptr != g_depthBuffer)
            {
                free(g_depthBuffer);
                g_depthBuffer = nullptr;
            }

            RECT clientRect = {};
            GetClientRect(hwnd, &clientRect);// left と top は常に 0

            int clientWidth = clientRect.right;
            int clientHeight = clientRect.bottom;

            BITMAPINFOHEADER bih = {};
            bih.biSize = sizeof(BITMAPINFOHEADER);
            bih.biWidth = clientWidth;
            bih.biHeight = clientHeight;
            bih.biPlanes = 1;
            bih.biBitCount = 32;
            bih.biCompression = BI_RGB;

            {
                HDC hWndDC = GetDC(hwnd);
                VOID* pvBits;
                g_hDibBm = CreateDIBSection(hWndDC, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &pvBits, NULL, 0);
                ReleaseDC(hwnd, hWndDC);
            }

            g_depthBuffer = malloc(sizeof(float) * clientWidth * clientHeight);

            InvalidateRect(hwnd, NULL, TRUE); // 再描画を要求
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps = {};
            BeginPaint(hwnd, &ps);

            HDC hMemDC = CreateCompatibleDC(ps.hdc);
            HGDIOBJ hPrevBm = SelectObject(hMemDC, g_hDibBm);
            DIBSECTION dibSection = {};
            GetObject(g_hDibBm, sizeof(DIBSECTION), &dibSection);

            int bmWidth = dibSection.dsBm.bmWidth;
            int bmHeight = std::abs(dibSection.dsBm.bmHeight);

            //
            // Windows GUI（クライアント領域、DIB）の座標系
            //
            //   (0, 0)
            //         +--------+-- +x
            //         |        |
            //         |        |
            //         +--------+
            //         |         (w, h)
            //       +y                
            //
            //
            // DIBのメモリを直接参照したとき座標系（メモリのレイアウトは上下反転）
            //
            //       +y                
            //         |         (w, h)
            //         +--------+
            //         |        |
            //         |        |
            //         +--------+-- +x
            //   (0, 0)
            //

            // レンダリング
            {
                Renderer renderer;

                renderer.setViewport(0, 0, bmWidth, bmHeight);

                ColorFrameBuffer colorFrameBuffer = {};
                colorFrameBuffer.addr = dibSection.dsBm.bmBits;
                colorFrameBuffer.width = bmWidth;
                colorFrameBuffer.height = bmHeight;
                colorFrameBuffer.widthBytes = dibSection.dsBm.bmWidthBytes;

                DepthFrameBuffer depthFrameBuffer = {};
                depthFrameBuffer.addr = g_depthBuffer;
                depthFrameBuffer.width = bmWidth;
                depthFrameBuffer.height = bmHeight;
                depthFrameBuffer.widthBytes = sizeof(float) * bmWidth;

                renderer.setFrameBuffer(&colorFrameBuffer, &depthFrameBuffer);

                renderer.clearFrameBuffer();

                RenderScene(&renderer);
            }

            // ウィンドウへ転送
            BitBlt(ps.hdc, 0, 0, bmWidth, bmHeight, hMemDC, 0, 0, SRCCOPY);

            SelectObject(hMemDC, hPrevBm);
            DeleteDC(hMemDC);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            if (NULL != g_hDibBm)
            {
                DeleteObject(g_hDibBm);
                g_hDibBm = NULL;
            }
            if (nullptr != g_depthBuffer)
            {
                free(g_depthBuffer);
                g_depthBuffer = nullptr;
            }
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    const TCHAR CLASS_NAME[] = TEXT("Sample Window Class");

    WNDCLASS wc = {};
    wc.lpfnWndProc = MyApp::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        TEXT("Sample Window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
