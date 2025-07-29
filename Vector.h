#pragma once
#include <cstdint>
#include <cmath>
#include <cassert>
#include <stdexcept>

namespace SoftwareRenderer
{
    //
    // Column vector
    //

    // v = | x |
    //     | y |
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

        float getLength() const
        {
            return std::sqrt(getLengthSquared());
        }

        Vector2 normalize() const
        {
            float length = getLength();
            if (length == 0.0f) return Vector2(0.0f, 0.0f); // 長さが0の場合はゼロベクトルを返す
            return Vector2(x / length, y / length);
        }

        // 内積
        float dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        // 外積
        float cross(const Vector2& other) const
        {
            return x * other.y - y * other.x;
        }

        // 加算演算子
        Vector2 operator+(const Vector2& other) const
        {
            return Vector2(x + other.x, y + other.y);
        }

        // 減算演算子
        Vector2 operator-(const Vector2& other) const
        {
            return Vector2(x - other.x, y - other.y);
        }

        // 乗算演算子（スカラー乗算）
        Vector2 operator*(float scalar) const
        {
            return Vector2(x * scalar, y * scalar);
        }

        // 割り算の演算子（スカラー乗算）
        Vector2 operator/(float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
        }

        // 左項にfloatを持つ乗算演算子
        friend Vector2 operator*(float scalar, const Vector2& vec)
        {
            return Vector2(vec.x * scalar, vec.y * scalar);
        }

        static Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t)
        {
            return ((1.0f - t) * v0) + (t * v1);
        }
    };


    //     | x |
    // v = | y |
    //     | z |
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
            default: throw std::out_of_range("Index out of range");
            }
        }

        float getLengthSquared() const
        {
            return x * x + y * y + z * z;
        }

        float getLength() const
        {
            return std::sqrt(getLengthSquared());
        }

        // 単位化
        Vector3 normalize() const
        {
            float length = getLength();
            if (length == 0.0f) return Vector3(0.0f, 0.0f, 0.0f); // 長さが0の場合はゼロベクトルを返す
            return Vector3(x / length, y / length, z / length);
        }

        // 内積
        float dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        // 外積
        Vector3 cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // 加算演算子
        Vector3 operator+(const Vector3& other) const
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        // 減算演算子
        Vector3 operator-(const Vector3& other) const
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        // 割り算の演算子（スカラー乗算）
        Vector3 operator/(float scalar) const
        {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        // 割り算の代入演算子
        Vector3& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        // 左項にfloatを持つ乗算演算子
        friend Vector3 operator*(float scalar, const Vector3& vec)
        {
            return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
        }

    };

    //     | x |
    // v = | y |
    //     | z |
    //     | w |
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
            default: throw std::out_of_range("Index out of range.");
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
            default: throw std::out_of_range("Index out of range.");
            }
        }

        // 加算演算子
        Vector4 operator+(const Vector4& other) const
        {
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        // 減算演算子
        Vector4 operator-(const Vector4& other) const
        {
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        // 乗算演算子（スカラー乗算）
        Vector4 operator*(float scalar) const
        {
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        // 除算演算子（スカラー除算）
        Vector4 operator/(float scalar) const
        {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        // 掛け算の代入演算子
        Vector4& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        // 割り算の代入演算子
        Vector4& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;
            return *this;
        }

        // 左項にfloatを持つ乗算演算子
        friend Vector4 operator*(float scalar, const Vector4& vec)
        {
            return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
        }

        static Vector4 Lerp(const Vector4& v0, const Vector4& v1, float t)
        {
            return ((1.0f - t) * v0) + (t * v1);
        }

    };
}
