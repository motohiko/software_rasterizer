#include "SamplerUtility.h"
#include "Modules\TextureOperations.h" 
#include <cmath>// floor, modff
#include <algorithm>//clamp

namespace SoftwareRasterizer
{
    Vector4 SamplerUtility::SampleTexture2d(const Sampler2D* sampler, const Vector2& texcoord)
    {

        FilterType filter = sampler->minFilter;

        switch(filter)
        {
            case FilterType::kPoint:
                return TextureUnit::SampleNearestPoint(sampler, texcoord);
            case FilterType::kBilinear:
                return TextureUnit::SampleBilinear(sampler, texcoord);
            default:
                return Vector4::kZero;
        }
    }
}
