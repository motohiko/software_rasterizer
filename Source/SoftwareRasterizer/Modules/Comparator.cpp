#include "Comparator.h"

namespace SoftwareRasterizer
{
    bool Comparator::Evaluate(float lhs, ComparisonFunc op, float rhs)
    {
        switch (op)
        {
        case ComparisonFunc::kNever:
            return false;
        case ComparisonFunc::kLess:
            return (lhs < rhs);
        case ComparisonFunc::kEqual:
            return (lhs == rhs);
        case ComparisonFunc::kLessEqual:
            return (lhs <= rhs);
        case ComparisonFunc::kGreater:
            return (lhs > rhs);
        case ComparisonFunc::kNotEqual:
            return (lhs != rhs);
        case ComparisonFunc::kGreaterEqual:
            return (lhs >= rhs);
        case ComparisonFunc::kAlways:
            return true;
        default:
            return false;
        }
    }

}