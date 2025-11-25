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

        void input(const ConstantBuffer* constantBuffer) { _constantBuffer = constantBuffer; }
        void input(const FragmentShaderProgram* fragmentShaderProgram) { _fragmentShaderProgram = fragmentShaderProgram; }
        void input(const QuadFragmentDataA* quadFragment) { _quadFragment = quadFragment; }

        void output(QuadFragmentDataA* quadFragment) { _outQuadFragment = quadFragment; }

        void execute();

    private:

        void executeShader(const FragmentDataA* fragment, Vector4* color) const;

    private:

        const ConstantBuffer* _constantBuffer;
        const FragmentShaderProgram* _fragmentShaderProgram;
        const QuadFragmentDataA* _quadFragment;

        QuadFragmentDataA* _outQuadFragment;

    };
}
