#pragma once

#include "..\Core\Types.h"

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
        FrontFaceType frontFacetype = FrontFaceType::kDefault;
        CullFaceType cullFaceType = CullFaceType::kDefault;

        //TODO: FillMode

    };
}