#pragma once

#include "FragmentShaderStageState.h"
#include "..\Types.h"
#include "..\Lib\Vector.h"

namespace SoftwareRasterizer
{

    class FragmentShaderStage
    {

    private:

        const FragmentShaderStageState* _fragmentShaderStageState;

    public:

        static void validateState(const FragmentShaderStageState* state);

        FragmentShaderStage(const FragmentShaderStageState* state);

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
