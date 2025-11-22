#include "Vector.h"
#include <cmath>

namespace Lib
{
    const Vector2 Vector2::kZero(0.0f, 0.0f);

    float Vector2::getNorm() const
    {
        return std::sqrt(getNormSquared());
    }

    const Vector3 Vector3::kZero(0.0f, 0.0f, 0.0f);

    float Vector3::getComponent(int index) const
    {
        switch (index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return 0.0f;
        }
    }

    float Vector3::getNorm() const
    {
        return std::sqrt(getNormSquared());
    }

    const Vector4 Vector4::kZero(0.0f, 0.0f, 0.0f, 0.0f);

    float Vector4::getComponent(int index) const
    {
        switch (index)
        {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        case 3: return w;
        default: return 0.0f;
        }
    }
}
