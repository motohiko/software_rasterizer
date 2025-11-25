#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
	class Comparator
	{

	public:

		static bool Evaluate(float lhs, ComparisonFuncType op, float rhs);

	};

}