#include "FragmentShaderStage.h"
#include <cassert>

namespace SoftwareRasterizer
{
    void FragmentShaderStage::validateState(const FragmentShaderProgram* state)
    {
        assert(state->fragmentShaderMain);
    }

    FragmentShaderStage::FragmentShaderStage()
    {
    }

    void FragmentShaderStage::executeShader(const Fragment* inputFragment, Vector4* outputColor) const
    {
        FragmentShaderInput fragmentShaderInput;
        fragmentShaderInput.uniformBlock = _constantBuffer->uniformBlock;
        fragmentShaderInput.fragCoord = Vector4(inputFragment->wndPosition, inputFragment->depth, inputFragment->invW);
        fragmentShaderInput.varyings = inputFragment->varyings;

        FragmentShaderOutput fragmentShaderOutput;
        _fragmentShaderProgram->fragmentShaderMain(&fragmentShaderInput, &fragmentShaderOutput);

        *outputColor = fragmentShaderOutput.fragColor;
    }
}
