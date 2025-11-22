#pragma once

#include <cstdint>

namespace SoftwareRasterizer
{
    class NormalizedConverter
    {

    public:

        static float NormalizeByte(uint8_t val);
        static uint8_t DenormalizeByte(float val);

        static float NormalizeU24(uint32_t val);
        static uint32_t DenormalizeU24(float val);
    };
}

