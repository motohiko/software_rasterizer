#pragma once

#include "..\Core\Types.h"
#include "..\State\VaryingEnabledBits.h"

namespace SoftwareRasterizer
{
    const int kClippingPointMaxNum = 7;

	class ClipStage
	{

	public:

        void setPrimitiveType(PrimitiveType primitiveType);
        void setVaryingEnabledBits(const VaryingEnabledBits* varyingEnabledBits);

        void clipPrimitive(const VertexDataB* vertices, int vertexNum, VertexDataB* clippedVertices, int* clippedVertiexNum) const;

	private:

        void clipPrimitiveLine(const VertexDataB* primitiveVertices, int primitiveVertexCount, VertexDataB* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;
        void clipPrimitiveTriangle(const VertexDataB* primitiveVertices, int primitiveVertexCount, VertexDataB* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;

	private:

        PrimitiveType _primitiveType;
        const VaryingEnabledBits* _varyingEnabledBits;

    };
}
