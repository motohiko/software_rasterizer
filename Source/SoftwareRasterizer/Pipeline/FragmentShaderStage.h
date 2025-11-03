#pragma once

#include "FragmentShaderStageState.h"
#include "..\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    class RenderingContext;

    class FragmentShaderStage
    {

    private:

        RenderingContext* _renderingContext;
        const FragmentShaderStageState* _fragmentShaderStageState;

    public:

        static void validateState(const FragmentShaderStageState* state);

        FragmentShaderStage(RenderingContext* renderingContext);

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
