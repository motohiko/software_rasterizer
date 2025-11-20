#pragma once

#include "..\Types.h"

namespace SoftwareRasterizer
{
    struct DepthRange
    {
        float depthRangeNearVal = 0.0f;
        float depthRangeFarVal = 1.0f;
    };
}
