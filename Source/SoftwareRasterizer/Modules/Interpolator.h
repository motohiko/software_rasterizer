#pragma once

#include "..\Core\Types.h"
#include "..\State\VaryingEnabledBits.h"

namespace SoftwareRasterizer
{

    // Barycentric coordinate system
    // r1=sarea(PBC)/sarea(ABC)
    // r2=sarea(APC)/sarea(ABC)
    // r3=sarea(ABP)/sarea(ABC)
    struct BarycentricCoord
    {
        float r1;
        float r2;
        float r3;
    };

	class Interpolator
	{

	public:

        static void InterpolateLinear(VertexDataB* p, const VertexDataB* a, const VertexDataB* b, float t, const VaryingEnabledBits* varyingEnabledBits);
        static void InterpolateLinear(VertexDataD* p, const VertexDataD* a, const VertexDataD* b, float t, const VaryingEnabledBits* varyingEnabledBits);

        static void InterpolateBarycentric(VertexDataD* p, const VertexDataD* a, const VertexDataD* b, const VertexDataD* c, const BarycentricCoord* baryCoord, const VaryingEnabledBits* varyingEnabledBits);
       
    };
}
