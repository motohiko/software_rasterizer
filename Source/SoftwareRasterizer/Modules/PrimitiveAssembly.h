#pragma once

#include "..\Core\Types.h"
#include "..\..\Lib\Vector.h"

namespace SoftwareRasterizer
{
	struct AssembledPrimitive
	{
		PrimitiveType primitiveType;
		uint16_t vertexIndices[3];
		int vertexNum;
	};

	class PrimitiveAssembly
	{

	public:

		void setPrimitiveType(PrimitiveType primitiveType);
		void setClipedVertices(const VertexDataB* vertices, int vertiexNum);
		void prepareDividPrimitive();
		bool readPrimitive(AssembledPrimitive* assembledPrimitive);

	private:

		PrimitiveType _primitiveType;
		//const ShadedVertex* _vertices;
		int _vertiexNum;
		int _readPrimitiveCount = 0;

	};
}
