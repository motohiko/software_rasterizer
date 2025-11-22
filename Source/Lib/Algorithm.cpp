#include "Algorithm.h"
#include <algorithm>

namespace Lib
{
    float NormalizeByte(uint8_t val)
    {
        return val / 255.0f;
    }

    uint8_t DenormalizeByte(float val)
    {
        return (uint8_t)(255.0f * std::clamp(val, 0.0f, 1.0f));
    }

}