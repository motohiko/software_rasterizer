#pragma once

#include "Common.h"
#include "Vector.h"

namespace SoftwareRenderer
{
    struct VertexShaderInput
    {
        const void* uniformBlock;
        const Vector4* attributes;
    };

    struct VertexShaderOutput
    {
        Vector4* position;// gl_Position, clip space coordinates
        Vector4* varyingVariables;
        int varyingVariableNum;
    };

    typedef void (*VertexShaderMainFunc)(const VertexShaderInput* input, VertexShaderOutput* output);

    class VertexShaderStage
    {

    private:

        const void* _uniformBlock = nullptr;
        VertexShaderMainFunc _vertexShaderMainFunc = nullptr;

    public:

        void setUniformBlock(const void* uniformBlock);
        void setVertexShaderProgram(VertexShaderMainFunc vertexShaderMainFunc);

        void executeShader(const AssembledVertex* assembledVertex, Vertex* vertex) const;

    };
}
