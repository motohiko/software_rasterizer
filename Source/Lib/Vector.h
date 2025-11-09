#pragma once

namespace Lib
{
    struct Vector2
    {
        float x;
        float y;

        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}

        float getLengthSquared() const
        {
            return x * x + y * y;
        }

        float getLength() const;

        Vector2 normalize() const;

        // 内積
        float dot(const Vector2& rhs) const
        {
            return x * rhs.x + y * rhs.y;
        }

        // 外積
        float cross(const Vector2& rhs) const
        {
            return x * rhs.y - y * rhs.x;
        }

       Vector2 operator+(const Vector2& rhs) const
        {
            return Vector2(x + rhs.x, y + rhs.y);
        }

        Vector2 operator-(const Vector2& rhs) const
        {
            return Vector2(x - rhs.x, y - rhs.y);
        }

        Vector2 operator*(float rhs) const
        {
            return Vector2(x * rhs, y * rhs);
        }

        Vector2 operator/(float rhs) const
        {
            return Vector2(x / rhs, y / rhs);
        }

        //friend Vector2 operator*(float lhs, const Vector2& rhs)
        //{
        //    return Vector2(rhs.x * lhs, rhs.y * lhs);
        //}

        static Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t)
        {
            return (v0 * (1.0f - t)) + (v1 * t);
        }
    };

    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() = default;
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        Vector2 getXY() const
        {
            return Vector2(x, y);
        }

        float getComponent(int index) const
        {
            switch (index)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return 0.0f;
            }
        }

        float getLengthSquared() const
        {
            return x * x + y * y + z * z;
        }

        float getLength() const;

        Vector3 normalize() const;

        // 内積
        float dot(const Vector3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        // 外積
        Vector3 cross(const Vector3& rhs) const
        {
            return Vector3(
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            );
        }

        Vector3 operator+(const Vector3& rhs) const
        {
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vector3 operator-(const Vector3& rhs) const
        {
            return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        Vector3 operator/(float rhs) const
        {
            return Vector3(x / rhs, y / rhs, z / rhs);
        }

        Vector3& operator/=(float rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            return *this;
        }

        friend Vector3 operator*(float lhs, const Vector3& rhs)
        {
            return Vector3(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
        }
    };

    struct Vector4
    {
        float x;
        float y;
        float z;
        float w;

        Vector4() = default;
        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector2& v2, float z, float w) : x(v2.x), y(v2.y), z(z), w(w) {}
        Vector4(const Vector3& v3, float w) : x(v3.x), y(v3.y), z(v3.z), w(w) {}

        Vector2 getXY() const
        {
            return Vector2(x, y);
        }

        Vector3 getXYZ() const
        {
            return Vector3(x, y, z);
        }

        void setComponent(int index, float value)
        {
            switch (index)
            {
            case 0: x = value;
            case 1: y = value;
            case 2: z = value;
            case 3: w = value;
            }
        }

        float getComponent(int index) const
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

        // 内積
        float dot(const Vector4& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
        }

        Vector4 operator+(const Vector4& rhs) const
        {
            return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }

        Vector4 operator-(const Vector4& rhs) const
        {
            return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }

        Vector4 operator*(float rhs) const
        {
            return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        Vector4 operator/(float rhs) const
        {
            return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
        }

        Vector4& operator*=(float rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;
            return *this;
        }

        Vector4& operator/=(float rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            w /= rhs;
            return *this;
        }

        //friend Vector4 operator*(float lhs, const Vector4& rhs)
        //{
        //    return Vector4(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs);
        //}

        static Vector4 Lerp(const Vector4& v0, const Vector4& v1, float t)
        {
            return (v0 * (1.0f - t)) + (v1 * t);
        }
    };
}
