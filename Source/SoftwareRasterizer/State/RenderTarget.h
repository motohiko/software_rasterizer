#pragma once

#include "Texture2D.h"
#include "..\Core\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    struct RenderTarget
    {
        Texture2D colorBuffer;// uint32_t[], BGRA
        Texture2D depthBuffer;// float[]
    };
}
