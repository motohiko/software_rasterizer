#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
	struct DepthState
	{
		bool depthTestEnabled = true;   // GL_DEPTH_TEST

		ComparisonFuncType depthFunc = ComparisonFuncType::kDefault;

		//TODO: DepthWriteMask 
	};
}