#include "VertexShaderStage.h"
#include "..\RenderingContext.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void VertexShaderStage::validateState(const VertexShaderStageState* state)
    {
        assert(state->uniformBlock);
        assert(state->vertexShaderMain);
    }

    VertexShaderStage::VertexShaderStage(RenderingContext* renderingContext) :
        _renderingContext(renderingContext),
        _vertexShaderStageState(&(renderingContext->_vertexShaderStageState))
    {
    }

    void VertexShaderStage::executeShader(const AttributeVertex* inputVertex, ShadedVertex* outputVertex) const
	{
        // 頂点座標をクリッピング空間座標に変換して、必要ならライティング用の情報を設定する

        VertexShaderInput vertexShaderInput;
        vertexShaderInput.uniformBlock = _vertexShaderStageState->uniformBlock;
        vertexShaderInput.attributes = inputVertex->attributes;

        VertexShaderOutput vertexShaderOutput;
        vertexShaderOutput.varyings = outputVertex->varyings;

        _vertexShaderStageState->vertexShaderMain(&vertexShaderInput, &vertexShaderOutput);
        assert(vertexShaderOutput.varyingNum < kMaxVaryings);

        outputVertex->clipPosition = vertexShaderOutput.position;
        outputVertex->varyingNum = vertexShaderOutput.varyingNum;
    }
}
