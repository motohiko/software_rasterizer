#pragma once

namespace Lib
{
    struct Vector2
    {
        float x;
        float y;

        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}

        static const Vector2 kZero;

        float getNormSquared() const
        {
            return (x * x) + (y * y);
        }

        float getNorm() const;

        // 内積
        float dot(const Vector2& rhs) const
        {
            return (x * rhs.x) + (y * rhs.y);
        }

        // 外積
        float cross(const Vector2& rhs) const
        {
            float z = (x * rhs.y) - (y * rhs.x);
            return z;
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

        static Vector2 Normalize(const Vector2& v)
        {
            float norm = v.getNorm();
            return (0.0f == norm) ? kZero : (v / norm);
        }

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

        static const Vector3 kZero;

        Vector2 getXY() const
        {
            return Vector2(x, y);
        }

        float getComponent(int index) const;

        float getNormSquared() const
        {
            return (x * x) + (y * y) + (z * z);
        }

        float getNorm() const;

        // 内積
        float dot(const Vector3& rhs) const
        {
            return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
        }

        // 外積
        Vector3 cross(const Vector3& rhs) const
        {
            return Vector3(
                (y * rhs.z) - (z * rhs.y),
                (z * rhs.x) - (x * rhs.z),
                (x * rhs.y) - (y * rhs.x)
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

        static Vector3 Normalize(const Vector3& v)
        {
            float norm = v.getNorm();
            return (0.0f == norm) ? kZero : (v / norm);
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

        static const Vector4 kZero;

        Vector2 getXY() const
        {
            return Vector2(x, y);
        }

        Vector3 getXYZ() const
        {
            return Vector3(x, y, z);
        }

        float getComponent(int index) const;

        // 加減算
        static Vector4 Add(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        }

        static Vector4 Subtract(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        }

        // スカラー積
        static Vector4 Scale(const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector4 Multiply(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        // 内積
        static float InnerProduct(const Vector4& lhs, const Vector4& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
        }

        // 線形補間
        static Vector4 Lerp(const Vector4& v0, const Vector4& v1, float t)
        {
            return (v0 * (1.0f - t)) + (v1 * t);
        }

        // 内積
        float dot(const Vector4& rhs) const { return InnerProduct(*this, rhs); }

        // 加減算
        Vector4 operator+(const Vector4& rhs) const { return Add(*this, rhs); }
        Vector4 operator-(const Vector4& rhs) const { return Subtract(*this, rhs); }

        // スカラー積
        Vector4 operator*(float rhs) const { return Scale(*this, rhs); }
        Vector4 operator/(float rhs) const { return Scale(*this, 1.0f / rhs); }
        Vector4& operator*=(float rhs) { *this = Scale(*this, rhs); return *this; }
        Vector4& operator/=(float rhs) { *this = Scale(*this, 1.0f / rhs); return *this; }

    };
}
