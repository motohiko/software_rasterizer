#include "FragmentShaderStage.h"
#include "..\RenderingContext.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void FragmentShaderStage::validateState(const FragmentShaderStageState* state)
    {
        assert(state->fragmentShaderMain);
    }

    FragmentShaderStage::FragmentShaderStage(RenderingContext* renderingContext) :
        _renderingContext(renderingContext),
        _fragmentShaderStageState(&(renderingContext->_fragmentShaderStageState))
    {
    }

    void FragmentShaderStage::executeShader(const Fragment* inputFragment, Vector4* outputColor) const
    {
        FragmentShaderInput fragmentShaderInput;
        fragmentShaderInput.uniformBlock = _fragmentShaderStageState->uniformBlock;
        fragmentShaderInput.fragCoord = Vector4(inputFragment->wndPosition, inputFragment->depth, inputFragment->invW);
        fragmentShaderInput.varyings = inputFragment->varyings;

        FragmentShaderOutput fragmentShaderOutput;
        _fragmentShaderStageState->fragmentShaderMain(&fragmentShaderInput, &fragmentShaderOutput);

        *outputColor = fragmentShaderOutput.fragColor;
    }
}
