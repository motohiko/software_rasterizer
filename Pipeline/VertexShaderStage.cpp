#include "VertexShaderStage.h"
#include <cassert>

namespace SoftwareRasterizer
{
    VertexShaderStage::VertexShaderStage(const VertexShaderStageState* state) :
        _vertexShaderStageState(state)
    {
    }

    void VertexShaderStage::validateState()
    {
        assert(_vertexShaderStageState->uniformBlock);
        assert(_vertexShaderStageState->vertexShaderMain);
    }


    void VertexShaderStage::executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const
	{
        VertexShaderInput vertexShaderInput;
        vertexShaderInput.uniformBlock = _vertexShaderStageState->uniformBlock;
        vertexShaderInput.attributes = inputVertex->attributes;

        VertexShaderOutput vertexShaderOutput;
        vertexShaderOutput.varyings = outputVertex->varyings;

        _vertexShaderStageState->vertexShaderMain(&vertexShaderInput, &vertexShaderOutput);
        assert(vertexShaderOutput.varyingNum < kMaxVaryings);

        outputVertex->clipSpacePosition = vertexShaderOutput.position;
        outputVertex->varyingNum = vertexShaderOutput.varyingNum;
    }
}
