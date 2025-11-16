#pragma once

#include "..\State\VertexShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class RenderingContext;

    class VertexShaderStage
    {

    private:

        RenderingContext* _renderingContext;
        const VertexShaderProgram* _vertexShaderProgram;
        const ConstantBuffer* _constantBuffer;

    public:

        static void validateState(const VertexShaderProgram* state);

        VertexShaderStage(RenderingContext* renderingContext);

        void executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const;

    };
}
