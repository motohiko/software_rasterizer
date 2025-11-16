#pragma once

#include "..\State\VertexShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Types.h"

namespace SoftwareRasterizer
{
    class VertexShaderStage
    {

    private:

        const ConstantBuffer* _constantBuffer;
        const VertexShaderProgram* _vertexShaderProgram;

    public:

        static void validateState(const VertexShaderProgram* state);

        VertexShaderStage();

        void input(const ConstantBuffer* constantBuffer) { _constantBuffer = constantBuffer; }
        void input(const VertexShaderProgram* vertexShaderProgram) { _vertexShaderProgram = vertexShaderProgram; }

        void executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const;

    };
}
