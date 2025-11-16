#include "TextureUtility.h"
#include "..\Lib\Algorithm.h"
#include <cstdint>
#include <cmath>// floor
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    struct RGBA
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    Vector4 TextureUtility::texture2D(const Sampler2D* sampler, const Vector2& uv)
    {
        int tx = (int)std::floor((uv.x * sampler->texture->width) + 0.5f);// 切り捨て
        int ty = (int)std::floor((uv.y * sampler->texture->height) + 0.5f);
        tx = std::clamp(tx, 0, sampler->texture->width - 1);
        ty = std::clamp(ty, 0, sampler->texture->height - 1);

        const RGBA* texel = ((const RGBA*)sampler->texture->addr) + ty * sampler->texture->width + tx;
        Vector4 color(
            Lib::NormalizeByte(texel->r),
            Lib::NormalizeByte(texel->g),
            Lib::NormalizeByte(texel->b),
            Lib::NormalizeByte(texel->a)
        );
        return color;
    }
}
