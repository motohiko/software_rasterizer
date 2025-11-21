#pragma once

#include "State\Texture2D.h"
#include "Types.h"

namespace SoftwareRasterizer
{

    struct Sampler2D
    {
        const Texture2D* texture;
    };

    class TextureUtility
    {

    public:

        static Vector4 texture2d(const Sampler2D* sampler, const Vector2& uv);// texture2D

    };

}
