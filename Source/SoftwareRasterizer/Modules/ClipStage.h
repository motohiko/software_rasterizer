#pragma once

#include "..\Core\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
    const int kClippingPointMaxNum = 7;

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

    };
}
