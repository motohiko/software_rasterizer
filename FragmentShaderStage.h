#pragma once

#include "Common.h"
#include "Vector.h"

namespace SoftwareRenderer
{
    struct FragmentShaderInput
    {
        const void* uniformBlock;
        Vector4 fragCoord;// gl_FragCoord, xy = window relative coordinates, z = depth, w = 1 / clip space w
        const Vector4* varyingVariables;
    };

    struct FragmentShaderOutput
    {
        Vector4 fragColor;// gl_FragColor, xyzw = rgba 
    };

    typedef void (*FragmentShaderMainFunc)(const FragmentShaderInput* input, FragmentShaderOutput* output);

    class FragmentShaderStage
    {

    private:

        const void* _uniformBlock = nullptr;
        FragmentShaderMainFunc _fragmentShaderMainFunc = nullptr;

    public:

        void setUniformBlock(const void* uniformBlock);
        void setFragmentShaderProgram(FragmentShaderMainFunc fragmentShaderMainFunc);

        void executeShader(const Fragment* fragment, Vector4* color) const;

    };
}
