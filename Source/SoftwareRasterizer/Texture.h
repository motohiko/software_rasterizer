#pragma once

#include "Types.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    struct Texture
    {
        const void* addr;
        int width;
        int height;
    };

    Vector4 SampleTexture(const Texture* tex, const Vector2& uv);

}
