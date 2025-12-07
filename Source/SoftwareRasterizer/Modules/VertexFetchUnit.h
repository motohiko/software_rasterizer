#pragma once

#include "..\State\InputLayout.h"
#include "..\State\VertexBuffers.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
	class VertexFetchUnit
	{

	public: 

		static void FetchVertex(const InputLayout* inputLayout, const VertexBuffers* vertexBuffers, int vertexIndex, VertexDataA* vertex);

	};
}
