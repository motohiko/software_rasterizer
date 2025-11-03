#include "Vector.h"
#include <cmath>

//namespace Lib
//{
    float Vector2::getLength() const
    {
        return std::sqrt(getLengthSquared());
    }

    Vector2 Vector2::normalize() const
    {
        float length = getLength();
        if (0.0f == length)
        {
            return Vector2(0.0f, 0.0f);
        }
        return Vector2(x / length, y / length);
    }

    float Vector3::getLength() const
    {
        return std::sqrt(getLengthSquared());
    }

    Vector3 Vector3::normalize() const
    {
        float length = getLength();
        if (0.0f == length)
        {
            return Vector3(0.0f, 0.0f, 0.0f);
        }
        return Vector3(x / length, y / length, z / length);
    }
//}
