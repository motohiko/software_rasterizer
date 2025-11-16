#pragma once

#include "..\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    struct FragmentShaderInput
    {
        const void* uniformBlock;
        Vector4 fragCoord;          // gl_FragCoord
        const Vector4* varyings;    // varying
    };

    struct FragmentShaderOutput
    {
        Vector4 fragColor;          // gl_FragColor 
    };

    typedef void (*FragmentShaderFuncPtr)(const FragmentShaderInput* input, FragmentShaderOutput* output);

    struct FragmentShaderProgram
    {
        FragmentShaderFuncPtr fragmentShaderMain = nullptr;
    };
}
