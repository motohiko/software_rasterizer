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

        VertexShaderStage(const VertexShaderStageState* state);

        void validateState();

        void executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const;

    };
}
