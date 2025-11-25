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
        // 頂点座標をクリッピング空間座標に変換して、必要ならライティング用の情報を設定する

        VertexShaderInput vertexShaderInput;
        vertexShaderInput.uniformBlock = _constantBuffer->uniformBlock;
        vertexShaderInput.attributes = inputVertex->attributes;

        VertexShaderOutput vertexShaderOutput;
        vertexShaderOutput.varyings = outputVertex->varyings;

        _vertexShaderProgram->vertexShaderMain(&vertexShaderInput, &vertexShaderOutput);
        assert(vertexShaderOutput.varyingNum < kMaxVaryings);

        outputVertex->clipPosition = vertexShaderOutput.position;
        outputVertex->varyingNum = vertexShaderOutput.varyingNum;
    }
}
