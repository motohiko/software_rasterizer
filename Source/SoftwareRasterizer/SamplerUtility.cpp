#include "SamplerUtility.h"

namespace SoftwareRasterizer
{
    Vector4 SamplerUtility::SampleTexture2d(const Sampler2D* sampler, const Vector2& texcoord)
    {

        FilterType filter = sampler->minFilter;

        switch(filter)
        {
            case FilterType::kPoint:
                return TextureMappingUnit::SampleNearestPoint(sampler, texcoord);
            case FilterType::kBilinear:
                return TextureMappingUnit::SampleBilinear(sampler, texcoord);
            default:
                return Vector4::kZero;
        }
    }
}
