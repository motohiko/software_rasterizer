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

    FragmentShaderStage::~FragmentShaderStage()
    {
    }

    void FragmentShaderStage::execute()
    {
        // TODO: 導関数対応＆MIN/MGA判定

        executeShader(&(_quadFragment->q00), &(_quadPixelData->q00));
        executeShader(&(_quadFragment->q01), &(_quadPixelData->q01));
        executeShader(&(_quadFragment->q10), &(_quadPixelData->q10));
        executeShader(&(_quadFragment->q11), &(_quadPixelData->q11));
    }

    void FragmentShaderStage::executeShader(const FragmentData* inputFragment, PixelData* outputPixel) const
    {
        FragmentShaderInput fragmentShaderInput;
        fragmentShaderInput.uniformBlock = _constantBuffer->uniformBlock;
        fragmentShaderInput.fragCoord = Vector4(inputFragment->wndCoord, inputFragment->depth, inputFragment->invW);
        fragmentShaderInput.varyings = inputFragment->varyings;

        FragmentShaderOutput fragmentShaderOutput;
        _fragmentShaderProgram->fragmentShaderMain(&fragmentShaderInput, &fragmentShaderOutput);

        outputPixel->color = fragmentShaderOutput.fragColor;
        outputPixel->depth = inputFragment->depth;
    }
}
