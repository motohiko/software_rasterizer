#include "NormalizedConverter.h"
#include <algorithm>// clamp

namespace SoftwareRasterizer
{
    static const float kU8Range = (float)0xff;
    static const float kU24Range = (float)0xffffff;

    float NormalizedConverter::NormalizeU8(uint8_t val)
    {
        return ((float)val) / kU8Range;
    }

    uint8_t NormalizedConverter::DenormalizeU8(float val)
    {
        float val01 = std::clamp(val, 0.0f, 1.0f);// saturate
        return (uint8_t)(kU8Range * val01);
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
