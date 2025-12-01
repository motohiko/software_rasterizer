#include "VertexShaderStage.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void VertexShaderStage::validateState(const VertexShaderProgram* state)
    {
        //assert(state->uniformBlock);
        assert(state->vertexShaderMain);
    }

    VertexShaderStage::VertexShaderStage()
    {
    }

    void VertexShaderStage::executeShader(const VertexDataA* inputVertex, VertexDataB* outputVertex) const
	{
        VertexShaderInput vertexShaderInput;
        vertexShaderInput.uniformBlock = _constantBuffer->uniformBlock;
        vertexShaderInput.attributes = inputVertex->attributes;

        VertexShaderOutput vertexShaderOutput;
        vertexShaderOutput.varyings = outputVertex->varyings;

        _vertexShaderProgram->vertexShaderMain(&vertexShaderInput, &vertexShaderOutput);
        assert(vertexShaderOutput.varyingNum < kMaxVaryings);

        outputVertex->clipCoord = vertexShaderOutput.position;
    }
}
