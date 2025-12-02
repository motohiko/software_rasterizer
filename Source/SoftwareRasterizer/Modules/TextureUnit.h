#pragma once

#include "..\State\Texture2D.h"
#include "..\Core\Types.h"

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

    class TextureUnit
    {

    public:

        static Vector4 SamplePoint(const Sampler2D* sampler, const IntVector2& texelCoord);
        static Vector4 SampleNearestPoint(const Sampler2D* sampler, const Vector2& texcoord);
        static Vector4 SampleBilinear(const Sampler2D* sampler, const Vector2& texcoord);

    };

}
