#include "FragmentShaderStage.h"
#include <cassert>

namespace MyApp
{

    void FragmentShaderStage::setUniformBlock(const void* uniformBlock)
    {
        _uniformBlock = uniformBlock;
    }

    void FragmentShaderStage::setFragmentShaderProgram(FragmentShaderMainFunc fragmentShaderMainFunc)
    {
        _fragmentShaderMainFunc = fragmentShaderMainFunc;
    }

    void FragmentShaderStage::executeShader(const Fragment* fragment, Vector4* color) const
    {
        FragmentShaderInput input;
        input.uniformBlock = _uniformBlock;
        input.fragCoord = Vector4(fragment->wrcPosition, fragment->depth, fragment->invW);
        input.varyingVariables = fragment->varyingVariables;

        FragmentShaderOutput output;

        assert(_fragmentShaderMainFunc);
        _fragmentShaderMainFunc(&input, &output);

        *color = output.fragColor;
    }
}