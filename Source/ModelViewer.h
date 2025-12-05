#pragma once

#include "SoftwareRasterizer\RenderingContext.h"

namespace Test
{
    using namespace SoftwareRasterizer;

    class ModelViewer
    {

    public:
        
        void onLButtonDrag(int xDelta, int yDelta);
        void onMouseWweel(int zDelta);
        void onKeyDown(int vk);
        void onPaint(RenderingContext* renderingContext);

    private:

        void renderScene(RenderingContext* renderingContext);

        struct MainCamera
        {
            float fovY = 60.0f * (3.14159265359f / 180.0f);

            float nearZ = 0.3f;
            float farZ = 10.0f;

            float focusPositionX = 0.0f;// 注視点
            float focusPositionY = 0.5f;
            float focusPositionZ = 0.0f;

            float angleX = 0.0f; // X軸回転角度
            float angleY = 0.0f; // Y軸回転角度

            float zoom = 3.0f;
        };

        MainCamera _camera;

    };
}
