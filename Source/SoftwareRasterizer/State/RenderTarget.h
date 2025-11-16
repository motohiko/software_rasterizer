#pragma once

#include "..\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    struct ResourceTexture2d
    {
        void* addr = nullptr;
        int width = 0;
        int height = 0;
        size_t widthBytes = 0;
    };

    struct RenderTarget
    {
        ResourceTexture2d colorBuffer;// uint32_t[], BGRA
        ResourceTexture2d depthBuffer;// float[]
    };
}
