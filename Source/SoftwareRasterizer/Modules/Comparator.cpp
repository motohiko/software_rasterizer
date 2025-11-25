#include "Comparator.h"

namespace SoftwareRasterizer
{
    bool Comparator::Evaluate(float lhs, ComparisonFuncType op, float rhs)
    {
        switch (op)
        {
        case ComparisonFuncType::kNever:
            return false;
        case ComparisonFuncType::kLess:
            return (lhs < rhs);
        case ComparisonFuncType::kEqual:
            return (lhs == rhs);
        case ComparisonFuncType::kLessEqual:
            return (lhs <= rhs);
        case ComparisonFuncType::kGreater:
            return (lhs > rhs);
        case ComparisonFuncType::kNotEqual:
            return (lhs != rhs);
        case ComparisonFuncType::kGreaterEqual:
            return (lhs >= rhs);
        case ComparisonFuncType::kAlways:
            return true;
        default:
            return false;
        }
    }

}