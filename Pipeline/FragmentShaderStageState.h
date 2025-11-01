#pragma once

#include "..\Vector.h"

namespace SoftwareRasterizer
{
    struct FragmentShaderInput
    {
        const void* uniformBlock;
        Vector4 fragCoord;// gl_FragCoord, xy = window relative coordinates, z = depth, w = 1 / clip space pos w
        const Vector4* varyings;
        bool helperInvocation;//  gl_helperinvocation
    };

    struct FragmentShaderOutput
    {
        Vector4 fragColor;// gl_FragColor, xyzw = rgba 
    };

    typedef void (*FragmentShaderFuncPtr)(const FragmentShaderInput* input, FragmentShaderOutput* output);

    struct FragmentShaderStageState
    {
        const void* uniformBlock = nullptr;
        FragmentShaderFuncPtr fragmentShaderMain = nullptr;
    };
}
