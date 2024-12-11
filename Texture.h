#pragma once

#include <cstdint>
#include <cmath>// floor

#include "Algorithm.h"// clamp
#include "Vector.h"

namespace MyApp
{
    struct Texture
    {
        const void* addr;
        int width;
        int height;
    };

    struct Texel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    inline Vector4 sampleTexture(const Texture* tex, const Vector2& uv)
    {
        int tx = (int)std::floor((uv.x * tex->width) + 0.5f);// 切り捨て
        int ty = (int)std::floor((uv.y * tex->height) + 0.5f);
        tx = clamp(tx, 0, tex->width - 1);
        ty = clamp(ty, 0, tex->height - 1);

        const Texel* texel = ((const Texel*)tex->addr) + ty * tex->width + tx;
        Vector4 color = Vector4(texel->r / 255.0f, texel->g / 255.0f, texel->b / 255.0f, 0.0f);
        return color;
    }

}
