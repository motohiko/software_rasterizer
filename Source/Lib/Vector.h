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

        static Vector2 Normalize(const Vector2& v)
        {
            float norm = v.getNorm();
            return (0.0f == norm) ? kZero : (v / norm);
        }

        static Vector2 Add(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
        }

        static Vector2 Subtract(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
        }

        static Vector2 ScalarProduct(const Vector2& lhs, float rhs)
        {
            return Vector2(lhs.x * rhs, lhs.y * rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector2 HadamardProduct(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
        }

        static float InnerProduct(const Vector2& lhs, const Vector2& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y);
        }

        // 疑似外積
        static float OuterProduct(const Vector2& lhs, const Vector2& rhs)
        {
            float z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
            return z;
        }

        static Vector2 Lerp(const Vector2& a, const Vector2& b, float t)
        {
            // a + ((b - a) * t);
            return Add(a, ScalarProduct(Subtract(b, a), t));
        }

        // 加減算
        Vector2 operator+(const Vector2& rhs) const { return Add(*this, rhs); }
        Vector2 operator-(const Vector2& rhs) const { return Subtract(*this, rhs); }

        // スカラー積
        Vector2 operator*(float rhs) const { return ScalarProduct(*this, rhs); }
        Vector2 operator/(float rhs) const { return ScalarProduct(*this, 1.0f / rhs); }

        // 要素ごとの積（アダマール積）
        Vector2 operator*(const Vector2& rhs) const { return HadamardProduct(*this, rhs); }

        // 内積
        float dot(const Vector2& rhs) const { return InnerProduct(*this, rhs); }

        // 疑似外積
        float cross(const Vector2& rhs) const { return OuterProduct(*this, rhs); }

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

        static Vector3 Normalize(const Vector3& v)
        {
            float norm = v.getNorm();
            return (0.0f == norm) ? kZero : (v / norm);
        }

        static Vector3 Add(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        static Vector3 Subtract(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }

        static Vector3 ScalarProduct(const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector3 HadamardProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }

        static float InnerProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
        }

        static Vector3 OuterProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(
                (lhs.y * rhs.z) - (lhs.z * rhs.y),
                (lhs.z * rhs.x) - (lhs.x * rhs.z),
                (lhs.x * rhs.y) - (lhs.y * rhs.x)
            );
        }

        static Vector3 Lerp(const Vector3& a, const Vector3& b, float t)
        {
            // a + ((b - a) * t);
            return Add(a, ScalarProduct(Subtract(b, a), t));
        }

        // 加減算
        Vector3 operator+(const Vector3& rhs) const { return Add(*this, rhs); }
        Vector3 operator-(const Vector3& rhs) const { return Subtract(*this, rhs); }

        // スカラー積
        Vector3 operator*(float rhs) const { return ScalarProduct(*this, rhs); }
        Vector3 operator/(float rhs) const { return ScalarProduct(*this, 1.0f / rhs); }
        Vector3& operator*=(float rhs) { *this = ScalarProduct(*this, rhs); return *this; }
        Vector3& operator/=(float rhs) { *this = ScalarProduct(*this, 1.0f / rhs); return *this; }

        // 要素ごとの積（アダマール積）
        Vector3 operator*(const Vector3& rhs) const { return HadamardProduct(*this, rhs); }

        // 内積
        float dot(const Vector3& rhs) const { return InnerProduct(*this, rhs); }

        // 外積
        Vector3 cross(const Vector3& rhs) const { return OuterProduct(*this, rhs); }

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

        static Vector4 Add(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        }

        static Vector4 Subtract(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        }

        static Vector4 ScalarProduct(const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector4 HadamardProduct(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        static float InnerProduct(const Vector4& lhs, const Vector4& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
        }

        static Vector4 Lerp(const Vector4& a, const Vector4& b, float t)
        {
            // a + ((b - a) * t);
            return Add(a, ScalarProduct(Subtract(b, a), t));
        }

        // 加減算
        Vector4 operator+(const Vector4& rhs) const { return Add(*this, rhs); }
        Vector4 operator-(const Vector4& rhs) const { return Subtract(*this, rhs); }

        // スカラー積
        Vector4 operator*(float rhs) const { return ScalarProduct(*this, rhs); }
        Vector4 operator/(float rhs) const { return ScalarProduct(*this, 1.0f / rhs); }
        Vector4& operator*=(float rhs) { *this = ScalarProduct(*this, rhs); return *this; }
        Vector4& operator/=(float rhs) { *this = ScalarProduct(*this, 1.0f / rhs); return *this; }

        // 要素ごとの積（アダマール積）
        Vector4 operator*(const Vector4& rhs) const { return HadamardProduct(*this, rhs); }

        // 内積
        float dot(const Vector4& rhs) const { return InnerProduct(*this, rhs); }

    };
}
