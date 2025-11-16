#pragma once

#include "..\State\FragmentShaderProgram.h"
#include "..\State\ConstantBuffer.h"
#include "..\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    class RenderingContext;

    class FragmentShaderStage
    {

    private:

        RenderingContext* _renderingContext;
        const FragmentShaderProgram* _fragmentShaderProgram;
        const ConstantBuffer* _constantBuffer;

    public:

        static void validateState(const FragmentShaderProgram* state);

        FragmentShaderStage(RenderingContext* renderingContext);

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
