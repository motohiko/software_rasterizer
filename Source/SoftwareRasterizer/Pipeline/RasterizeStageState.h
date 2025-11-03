#pragma once

#include "..\Types.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    // normalized device coordinates
    struct NdcVertex
    {
        Vector3 ndcPosition;
    };

    struct RasterVertex
    {
        Vector2 wrcPosition;// window relative coordinate
        float depth;
        float invW;
        Vector4 varyingVariablesDividedByW[kMaxVaryings];
        int varyingNum;
    };

    struct RasterizeStageState
    {
        int viewportX = 0;
        int viewportY = 0;
        int viewportWidth = 0;
        int viewportHeight = 0;

        float depthRangeNearVal = 0.0;
        float depthRangeFarVal = 1.0f;
    };
}