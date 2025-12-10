#pragma once

#include <cstdint>

namespace SoftwareRasterizer
{
    class NormalizedConverter
    {

    public:

        static float NormalizeU8(uint8_t val);
        static uint8_t DenormalizeU8(float val);

        static float NormalizeU24(uint32_t val);
        static uint32_t DenormalizeU24(float val);
    };
}

