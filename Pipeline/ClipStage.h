#pragma once

#include "..\Types.h"
#include "..\Vector.h"

namespace SoftwareRasterizer
{
    const int kTriangleClippingPointMaxNum = 7;

    const int kClippingPlaneNum = 6;

    struct ClippingPlaneParameter
    {
        int vectorComponentIndex;// 0=x, 1=y, 2=z
        float sign;
    };

    const ClippingPlaneParameter kClipPlaneParameters[kClippingPlaneNum] =
    {
        { 0, -1.0f },// left
        { 0,  1.0f },// right
        { 1, -1.0f },// bottom
        { 1,  1.0f },// top
        { 2, -1.0f },// near
        { 2,  1.0f } // far
    };

	class ClipStage
	{

	private:

        PrimitiveType _primitiveType;

	public:

        void setPrimitiveType(PrimitiveType primitiveType);

        void clipPrimitive(const ShadedVertex* vertices, int vertexNum, ShadedVertex* clippedVertices, int* clippedVertiexNum) const;

	private:

        void clipPrimitiveLine(const ShadedVertex* primitiveVertices, int primitiveVertexCount, ShadedVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;
        void clipPrimitiveTriangle(const ShadedVertex* primitiveVertices, int primitiveVertexCount, ShadedVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;

        void lerpVertex(ShadedVertex* dst, const ShadedVertex& p0, const ShadedVertex& p1, float t) const
        {
            // ¦ w ‚à xyz ‚Æ“¯—l‚ÉüŒ`•âŠÔ‚·‚é

            int varyingNum = p0.varyingNum;// TODO:

            dst->clipSpacePosition = Vector4::Lerp(p0.clipSpacePosition, p1.clipSpacePosition, t);
            for (int i = 0; i < varyingNum; ++i)
            {
                dst->varyings[i] = Vector4::Lerp(p0.varyings[i], p1.varyings[i], t);
            }
            dst->varyingNum = varyingNum;
        }
	};
}
