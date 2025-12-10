#pragma once

#include "Modules\TextureMappingUnit.h"
#include "State\Texture2D.h"
#include "Core\Types.h"

namespace SoftwareRasterizer
{

    class SamplerUtility
    {

    public:

        static Vector4 SampleTexture2d(const Sampler2D* sampler, const Vector2& texcoord);// texture2D

    };

}
