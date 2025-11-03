#pragma once

#include "VertexShaderStageState.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class RenderingContext;

    class VertexShaderStage
    {

    private:

        RenderingContext* _renderingContext;
        const VertexShaderStageState* _vertexShaderStageState;

    public:

        static void validateState(const VertexShaderStageState* state);

        VertexShaderStage(RenderingContext* renderingContext);

        void executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const;

    };
}
