#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct VertexShaderInput
    {
        const void* uniformBlock;
        const Vector4* attributes;
        int vertexId;       // gl_VertexID
    };

    struct VertexShaderOutput
    {
        Vector4 position;   // gl_Position
        Vector4* varyings;
    };

    typedef void (*VertexShaderFuncPtr)(const VertexShaderInput* input, VertexShaderOutput* output);

    struct VertexShaderProgram
    {
        VertexShaderFuncPtr vertexShaderMain = nullptr;
    };
}
