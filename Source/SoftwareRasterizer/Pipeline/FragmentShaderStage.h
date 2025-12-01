#pragma once

#include "..\State\FragmentShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    class FragmentShaderStage
    {

    public:

        static void validateState(const FragmentShaderProgram* state);

        FragmentShaderStage();
        ~FragmentShaderStage();

        void input(const ConstantBuffer* constantBuffer) { _constantBuffer = constantBuffer; }
        void input(const FragmentShaderProgram* fragmentShaderProgram) { _fragmentShaderProgram = fragmentShaderProgram; }
        void input(const QuadFragmentData* quadFragment) { _quadFragment = quadFragment; }

        void output(QuadPixelData* quadPixelData) { _quadPixelData = quadPixelData; }

        void execute();

    private:

        void executeShader(const FragmentData* inputFragment, PixelData* outputPixel) const;

    private:

        // input
        const ConstantBuffer* _constantBuffer;
        const FragmentShaderProgram* _fragmentShaderProgram;
        const QuadFragmentData* _quadFragment;

        // output
        QuadPixelData* _quadPixelData;

    };
}
