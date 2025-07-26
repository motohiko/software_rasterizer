#include "VertexShaderStage.h"
#include <cassert>

namespace MyApp
{
    void VertexShaderStage::setUniformBlock(const void* uniformBlock)
    {
        _uniformBlock = uniformBlock;
    }

    void VertexShaderStage::setVertexShaderProgram(VertexShaderMainFunc vertexShaderMainFunc)
    {
        _vertexShaderMainFunc = vertexShaderMainFunc;
    }

    void VertexShaderStage::executeShader(const AssembledVertex* assembledVertex, Vertex* vertex) const
	{
        VertexShaderInput vertexShaderInput;
        vertexShaderInput.uniformBlock = _uniformBlock;
        vertexShaderInput.attributes = assembledVertex->attributes;

        VertexShaderOutput vertexShaderOutput;
        vertexShaderOutput.position = &(vertex->clipSpacePosition);
        vertexShaderOutput.varyingVariables = vertex->varyingVariables;

        assert(_vertexShaderMainFunc);
        _vertexShaderMainFunc(&vertexShaderInput, &vertexShaderOutput);
	}
}
