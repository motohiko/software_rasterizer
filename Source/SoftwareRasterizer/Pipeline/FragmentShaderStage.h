#pragma once

#include "..\State\FragmentShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    class FragmentShaderStage
    {

    private:

        const ConstantBuffer* _constantBuffer;
        const FragmentShaderProgram* _fragmentShaderProgram;

    public:

        static void validateState(const FragmentShaderProgram* state);

        FragmentShaderStage();

        void input(const ConstantBuffer* constantBuffer) { _constantBuffer = constantBuffer; }
        void input(const FragmentShaderProgram* fragmentShaderProgram) { _fragmentShaderProgram = fragmentShaderProgram; }

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
