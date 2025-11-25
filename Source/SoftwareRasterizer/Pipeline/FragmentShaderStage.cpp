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

    void FragmentShaderStage::execute()
    {
        executeShader(_quadFragment->getQ00(), &(_outQuadFragment->getQ00()->color));
        executeShader(_quadFragment->getQ01(), &(_outQuadFragment->getQ01()->color));
        executeShader(_quadFragment->getQ10(), &(_outQuadFragment->getQ10()->color));
        executeShader(_quadFragment->getQ11(), &(_outQuadFragment->getQ11()->color));
    }

    void FragmentShaderStage::executeShader(const FragmentDataA* inputFragment, Vector4* outputColor) const
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
