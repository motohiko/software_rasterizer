#pragma once

#include "VertexShaderStageState.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class VertexShaderStage
    {

    private:

        const VertexShaderStageState* _vertexShaderStageState;

    public:

        static void validateState(const VertexShaderStageState* state);

        VertexShaderStage(const VertexShaderStageState* state);

        void executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const;

    };
}
