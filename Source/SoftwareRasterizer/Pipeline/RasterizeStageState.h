#pragma once

#include "..\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    struct RasterizeStageState
    {
        int viewportX = 0;
        int viewportY = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;

        float depthRangeNearVal = 0.0;
        float depthRangeFarVal = 1.0f;
        
        FrontFaceType frontFacetype = FrontFaceType::kDefault;
        CullFaceType cullFaceType = CullFaceType::kDefault;
    };
}