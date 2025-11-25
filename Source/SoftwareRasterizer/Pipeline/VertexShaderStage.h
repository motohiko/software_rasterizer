#pragma once

#include "..\State\VertexShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    class VertexShaderStage
    {

    public:

        static void validateState(const VertexShaderProgram* state);

        VertexShaderStage();

        void input(const ConstantBuffer* constantBuffer) { _constantBuffer = constantBuffer; }
        void input(const VertexShaderProgram* vertexShaderProgram) { _vertexShaderProgram = vertexShaderProgram; }

        void executeShader(const VertexDataA* inputVertex, VertexDataB* outputVertex) const;

    private:

        const ConstantBuffer* _constantBuffer;
        const VertexShaderProgram* _vertexShaderProgram;

    };
}
