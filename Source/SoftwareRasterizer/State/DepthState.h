#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    enum class ComparisonType
    {
        kNone,
        kNever,         // GL_NEVER
        kLess,          // GL_LESS
        kEqual,         // GL_EQUAL
        kLessEqual,     // GL_LEQUAL
        kGreater,       // GL_GREATER
        kNotEqual,      // GL_NOTEQUAL
        kGreaterEqual,  // GL_GEQUAL
        kAlways,        // GL_ALWAYS
        kDefault = kLess,
    };

	struct DepthState
	{
		bool depthEnable = true;

		ComparisonType depthFunc = ComparisonType::kDefault;

		//TODO: DepthWriteMask 
	};
}