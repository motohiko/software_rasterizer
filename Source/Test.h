#pragma once

#include "SoftwareRasterizer\RenderingContext.h"

namespace Test
{
    using namespace SoftwareRasterizer;

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

    class ModelViewer
    {

    public:

        void RenderScene(RenderingContext* _renderingContext, const TestCamera* camera);


    };
}
