#pragma once

#include "Types.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    struct Texture2D
    {
        const void* addr;

        //internalformat = 4

        int width;
        int height;

        //format = GL_RGBA

    };

    struct Sampler2D
    {
        const Texture2D* texture;
    };

    Vector4 texture2D(const Sampler2D* sampler, const Vector2& uv);

}
