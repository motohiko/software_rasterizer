#pragma once

namespace SoftwareRasterizer
{
    struct ClearParam
    {
        float clearColorR = 0.0f;
        float clearColorG = 0.0f;
        float clearColorB = 0.0f;
        float clearColorA = 0.0f;

        float clearDepth = 1.0f;
    };
};
