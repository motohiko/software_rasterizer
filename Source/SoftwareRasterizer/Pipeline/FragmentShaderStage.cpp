#include "FragmentShaderStage.h"
#include "..\RenderingContext.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void FragmentShaderStage::validateState(const FragmentShaderProgram* state)
    {
        assert(state->fragmentShaderMain);
    }

    FragmentShaderStage::FragmentShaderStage(RenderingContext* renderingContext) :
        _renderingContext(renderingContext),
        _fragmentShaderProgram(&(renderingContext->_fragmentShaderProgram)),
        _constantBuffer(&(renderingContext->_constantBuffer))
    {
    }

    void FragmentShaderStage::executeShader(const Fragment* inputFragment, Vector4* outputColor) const
    {
        FragmentShaderInput input;
        input.uniformBlock = _constantBuffer->uniformBlock;
        input.fragCoord = Vector4(inputFragment->wndPosition, inputFragment->depth, inputFragment->invW);
        input.varyings = inputFragment->varyings;

        FragmentShaderOutput output;
        _fragmentShaderProgram->fragmentShaderMain(&input, &output);

        *outputColor = output.fragColor;
    }
}
