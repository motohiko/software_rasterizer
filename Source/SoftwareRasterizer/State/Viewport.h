#pragma once

#include "..\Types.h"

namespace SoftwareRasterizer
{
    struct Viewport
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;

        float depthRangeNearVal = 0.0f;
        float depthRangeFarVal = 1.0f;
    };
}