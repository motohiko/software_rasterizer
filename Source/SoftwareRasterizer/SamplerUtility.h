#pragma once

#include "State\Texture2D.h"
#include "Types.h"

namespace SoftwareRasterizer
{

    enum FilterType
    {
        kPoint,     // GL_NEAREST
        kBilinear,  // GL_LINEAR
    };

    struct Sampler2D
    {
        const Texture2D* texture;

        FilterType minFilter;// GL_TEXTURE_MIN_FILTER
        FilterType magFilter;// GL_TEXTURE_MAG_FILTER

    };

    class SamplerUtility
    {

    private:

        static Vector4 SamplePoint(const Texture2D* texture, int tx, int ty);
        static Vector4 SampleNearestPoint(const Sampler2D* sampler, const Vector2& uv);
        static Vector4 SampleBilinear(const Sampler2D* sampler, const Vector2& uv);

    public:

        static Vector4 SampleTexture2d(const Sampler2D* sampler, const Vector2& uv);// texture2D

    };

}
