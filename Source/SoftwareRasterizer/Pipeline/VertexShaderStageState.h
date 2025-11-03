#pragma once

#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    struct VertexShaderInput
    {
        const void* uniformBlock;
        const Vector4* attributes;
    };

    struct VertexShaderOutput
    {
        Vector4 position;// gl_Position, clip space coordinates
        Vector4* varyings;
        int varyingNum;
    };

    typedef void (*VertexShaderFuncPtr)(const VertexShaderInput* input, VertexShaderOutput* output);

    struct VertexShaderStageState
    {
        const void* uniformBlock = nullptr;
        VertexShaderFuncPtr vertexShaderMain = nullptr;
    };
}
