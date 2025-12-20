#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
	class CompareTest
	{

	public:

		static bool Perform(ComparisonFunc op, float lhs, float rhs);

	};

}