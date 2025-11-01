#include "FragmentShaderStage.h"
#include <cassert>

namespace SoftwareRasterizer
{
    FragmentShaderStage::FragmentShaderStage(const FragmentShaderStageState* state) :
        _fragmentShaderStageState(state)
    {
    }

    void FragmentShaderStage::validateState()
    {
        assert(_fragmentShaderStageState->fragmentShaderMain);
    }

    void FragmentShaderStage::executeShader(const Fragment* inputFragment, Vector4* outputColor) const
    {
        FragmentShaderInput fragmentShaderInput;
        fragmentShaderInput.uniformBlock = _fragmentShaderStageState->uniformBlock;
        fragmentShaderInput.fragCoord = Vector4(inputFragment->wrcPosition, inputFragment->depth, inputFragment->invW);
        fragmentShaderInput.varyings = inputFragment->varyings;

        FragmentShaderOutput fragmentShaderOutput;
        _fragmentShaderStageState->fragmentShaderMain(&fragmentShaderInput, &fragmentShaderOutput);

        *outputColor = fragmentShaderOutput.fragColor;
    }
}
