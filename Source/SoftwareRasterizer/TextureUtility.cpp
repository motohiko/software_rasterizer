#include "TextureUtility.h"
#include "Pipeline\TextureOperations.h" 
#include <cmath>// floor
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    Vector4 TextureUtility::texture2d(const Sampler2D* sampler, const Vector2& uv)
    {
        const Texture2D* texture = sampler->texture;

        int tx = (int)std::floor((uv.x * texture->width) + 0.5f);// Ø‚èŽÌ‚Ä
        int ty = (int)std::floor((uv.y * texture->height) + 0.5f);
        tx = std::clamp(tx, 0, texture->width - 1);
        ty = std::clamp(ty, 0, texture->height - 1);

        return TextureOperations::fetchTexelColor(sampler->texture, tx, ty);
    }
}
