#pragma once

#include "Texture2D.h"

namespace SoftwareRasterizer
{
    struct RenderTarget
    {
        Texture2D colorBuffer;// uint32_t[], BGRA
        Texture2D depthBuffer;// float[]
    };
}
