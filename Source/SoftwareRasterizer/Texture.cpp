#include "Texture.h"
#include "..\Lib\Algorithm.h"
#include <cstdint>
#include <cmath>// floor
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    struct Texel
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    Vector4 SampleTexture(const Texture* tex, const Vector2& uv)
    {
        int tx = (int)std::floor((uv.x * tex->width) + 0.5f);// Ø‚èŽÌ‚Ä
        int ty = (int)std::floor((uv.y * tex->height) + 0.5f);
        tx = std::clamp(tx, 0, tex->width - 1);
        ty = std::clamp(ty, 0, tex->height - 1);

        const Texel* texel = ((const Texel*)tex->addr) + ty * tex->width + tx;
        Vector4 color(
            Lib::NormalizeByte(texel->r),
            Lib::NormalizeByte(texel->g),
            Lib::NormalizeByte(texel->b),
            Lib::NormalizeByte(texel->a)
        );
        return color;
    }
}
