#include "Interpolator.h"
#include <cmath>// lerp

namespace SoftwareRasterizer
{
    void Interpolator::InterpolateLinear(VertexDataB* p, const VertexDataB* a, const VertexDataB* b, float t, const VaryingIndexState* varyingIndexState)
    {
        p->clipCoord = Vector4(
            std::lerp(a->clipCoord.x, b->clipCoord.x, t),
            std::lerp(a->clipCoord.y, b->clipCoord.y, t),
            std::lerp(a->clipCoord.z, b->clipCoord.z, t),
            std::lerp(a->clipCoord.w, b->clipCoord.w, t)
        );
        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                const Vector4& av = a->varyings[i];
                const Vector4& bv = b->varyings[i];
                p->varyings[i] = Vector4(
                    std::lerp(av.x, bv.x, t),
                    std::lerp(av.y, bv.y, t),
                    std::lerp(av.z, bv.z, t),
                    std::lerp(av.w, bv.w, t)
                );
            }
        }
    }

    void Interpolator::InterpolateLinear(VertexDataD* p, const VertexDataD* a, const VertexDataD* b, float t, const VaryingIndexState* varyingIndexState)
    {
        p->wndCoord = Vector2(
            std::lerp(a->wndCoord.x, b->wndCoord.x, t),
            std::lerp(a->wndCoord.y, b->wndCoord.y, t)
        );
        p->depth = std::lerp(a->depth, b->depth, t);
        p->invW = std::lerp(a->invW, b->invW, t);
        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                const Vector4& av = a->varyingsDividedByW[i];
                const Vector4& bv = b->varyingsDividedByW[i];
                p->varyingsDividedByW[i] = Vector4(
                    std::lerp(av.x, bv.x, t),
                    std::lerp(av.y, bv.y, t),
                    std::lerp(av.z, bv.z, t),
                    std::lerp(av.w, bv.w, t)
                );
            }
        }
    }

    void Interpolator::InterpolateBarycentric(VertexDataD* p, const VertexDataD* a, const VertexDataD* b, const VertexDataD* c, const BarycentricCoord* baryCoord, const VaryingIndexState* varyingIndexState)
    {
        float r1 = baryCoord->r1;
        float r2 = baryCoord->r2;
        float r3 = baryCoord->r3;
        p->wndCoord = (a->wndCoord * r1) + (b->wndCoord * r2) + (c->wndCoord * r3);
        p->depth = (a->depth * r1) + (b->depth * r2) + (c->depth * r3);
        p->invW = (a->invW * r1) + (b->invW * r2) + (c->invW * r3);
        for (int i = 0; i < kMaxVaryings; i++)
        {
            if (varyingIndexState->enabledVaryingIndexBits & (1u << i))
            {
                const Vector4& av = a->varyingsDividedByW[i];
                const Vector4& bv = b->varyingsDividedByW[i];
                const Vector4& cv = c->varyingsDividedByW[i];
                p->varyingsDividedByW[i] = (av * r1) + (bv * r2) + (cv * r3);
            }
        }
    }

}
