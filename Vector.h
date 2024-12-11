#pragma once
#include <cstdint>
#include <cmath>
#include <cassert>
#include <stdexcept>

namespace MyApp
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

        // 内積を求める
        float dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        // 2次元ベクトルとしての外積を求める
        float cross(const Vector2& other) const
        {
            return x * other.y - y * other.x;
        }

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
            float len = getLength();
            assert(0 < len);
            return *this / len;
        }

        // 減算演算子のオーバーロード
        Vector2 operator-(const Vector2& other) const
        {
            return Vector2(x - other.x, y - other.y);
        }

        // 割り算の演算子オーバーロード（スカラー乗算）
        Vector2 operator/(float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
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

        // 単位化ベクトル
        Vector3 normalize() const
        {
            float length = std::sqrt(x * x + y * y + z * z);
            if (length == 0) return Vector3(0.0f, 0.0f, 0.0f); // 長さが0の場合はゼロベクトルを返す
            return Vector3(x / length, y / length, z / length);
        }

        // 内積
        float dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }

        // 3次元ベクトルとしての外積
        Vector3 cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            );
        }

        // 加算演算子のオーバーロード
        Vector3 operator+(const Vector3& other) const
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }

        // 減算演算子のオーバーロード
        Vector3 operator-(const Vector3& other) const
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }

        // 割り算の演算子オーバーロード（スカラー乗算）
        Vector3 operator/(float scalar) const
        {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }

        // 割り算の代入演算子オーバーロード
        Vector3& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        // フレンド関数として左項にfloatを持つ乗算演算子のオーバーロード
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

        // Vector2への暗黙的なキャスト演算子をオーバーロード
        operator Vector2() const
        {
            return Vector2(x, y);
        }

        // Vector3への暗黙的なキャスト演算子をオーバーロード
        operator Vector3() const
        {
            return Vector3(x, y, z);
        }

        // 加算演算子のオーバーロード
        Vector4 operator+(const Vector4& other) const
        {
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
        }

        // 減算演算子のオーバーロード
        Vector4 operator-(const Vector4& other) const
        {
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
        }

        // 乗算演算子のオーバーロード（スカラー乗算）
        Vector4 operator*(float scalar) const
        {
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        // 除算演算子のオーバーロード（スカラー除算）
        Vector4 operator/(float scalar) const
        {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        // フレンド関数として左項にfloatを持つ乗算演算子のオーバーロード
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
