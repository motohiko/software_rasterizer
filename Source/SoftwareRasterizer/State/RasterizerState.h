#pragma once

#include "..\Types.h"

namespace SoftwareRasterizer
{
    enum class FrontFaceType
    {
        kNone,
        kClockwise,         //GL_CW
        kCounterClockwise,  // GL_CCW
        kDefault = kCounterClockwise,
    };

    enum class CullFaceType
    {
        kNone,
        kFront,         // GL_FRONT
        kBack,          // GL_BACK
        kFrontAndBack,  // GL_FRONT_AND_BACK
        kDefault = kBack,
    };

    struct RasterizerState
    {
        //TODO: FillMode

        FrontFaceType frontFacetype = FrontFaceType::kDefault;
        CullFaceType cullFaceType = CullFaceType::kDefault;

        float depthRangeNearVal = 0.0;
        float depthRangeFarVal = 1.0f;
    };
}