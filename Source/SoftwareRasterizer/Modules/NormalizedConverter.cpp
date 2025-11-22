#include "NormalizedConverter.h"
#include <algorithm>// clamp

namespace SoftwareRasterizer
{
    static const float kByteRange = (float)0xff;
    static const float kU24Range = (float)0xffffff;

    float NormalizedConverter::NormalizeByte(uint8_t val)
    {
        return ((float)val) / kByteRange;
    }

    uint8_t NormalizedConverter::DenormalizeByte(float val)
    {
        float val01 = std::clamp(val, 0.0f, 1.0f);// saturate
        return (uint8_t)(kByteRange * val01);
    }

    float NormalizedConverter::NormalizeU24(uint32_t val)
    {
        return ((float)val) / kU24Range;
    }

    uint32_t NormalizedConverter::DenormalizeU24(float val)
    {
        float val01 = std::clamp(val, 0.0f, 1.0f);// saturate
        return (uint32_t)(kU24Range * val01);
    }

}
