#pragma once

#include <cstdint>

namespace SoftwareRasterizer
{
    class DataConversionRule
    {

    public:

        static float ConvertUnorm8ToFloat32(uint8_t val);
        static uint8_t ConvertFloat32ToUnorm8(float val);

        static float ConvertUnorm24ToFloat32(uint32_t val);
        static uint32_t ConvertFloat32ToUnorm24(float val);
    };
}

