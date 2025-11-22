#pragma once

#include "..\Core\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    struct IndexBuffer
    {
        const uint16_t* indices = nullptr;
        int indexNum = 0;
    };
}
