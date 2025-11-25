#pragma once

namespace SoftwareRasterizer
{
    enum class FrontFaceMode
    {
        kNone,
        kClockwise,         // GL_CW
        kCounterClockwise,  // GL_CCW
        kDefault = kCounterClockwise,
    };

    enum class CullFaceMode
    {
        kNone,
        kFront,         // GL_FRONT
        kBack,          // GL_BACK
        kFrontAndBack,  // GL_FRONT_AND_BACK
        kDefault = kBack,
    };

    struct RasterizerState
    {
        FrontFaceMode frontFaceMode = FrontFaceMode::kDefault;
        CullFaceMode cullFaceMode = CullFaceMode::kDefault;
    };
}