#pragma once

#include "FragmentShaderStageState.h"
#include "..\Types.h"
#include "..\Vector.h"

namespace SoftwareRasterizer
{

    class FragmentShaderStage
    {

    private:

        const FragmentShaderStageState* _fragmentShaderStageState;

    public:

        FragmentShaderStage(const FragmentShaderStageState* state);

        void validateState();

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
