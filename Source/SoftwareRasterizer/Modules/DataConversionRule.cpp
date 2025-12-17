#include "DataConversionRule.h"
#include <algorithm>// clamp

namespace SoftwareRasterizer
{
    // UNORM8 -> FLOAT32
    float DataConversionRule::ConvertUnorm8ToFloat32(uint8_t val)
    {
        return ((float)val) / (float)0xff;
    }

    // FLOAT32 -> UNORM8
    uint8_t DataConversionRule::ConvertFloat32ToUnorm8(float val)
    {
        float val01 = std::clamp(val, 0.0f, 1.0f);
        return (uint8_t)(((float)0xff) * val01);
    }

    // UNORM24 -> FLOAT32
    float DataConversionRule::ConvertUnorm24ToFloat32(uint32_t val)
    {
        return ((float)val) / (float)0xffffff;
    }

    // FLOAT32 -> UNORM24
    uint32_t DataConversionRule::ConvertFloat32ToUnorm24(float val)
    {
        float val01 = std::clamp(val, 0.0f, 1.0f);
        return (uint32_t)(((float)0xffffff) * val01);
    }

}
