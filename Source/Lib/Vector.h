#pragma once

namespace Lib
{
    struct Vector2
    {
        float x;
        float y;

        static const Vector2 kZero;

        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}

        float getNormSquared() const
        {
            return (x * x) + (y * y);
        }

        float getNorm() const;

        static Vector2 Normalize(const Vector2& v);

        static Vector2 Add(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
        }

        static Vector2 Subtract(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
        }

        static Vector2 ScaleByScalar(const Vector2& lhs, float rhs)
        {
            return Vector2(lhs.x * rhs, lhs.y * rhs);
        }

        static Vector2 DivideByScalar(const Vector2& lhs, float rhs)
        {
            return Vector2(lhs.x / rhs, lhs.y / rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector2 ComputeElementwiseProduct(const Vector2& lhs, const Vector2& rhs)
        {
            return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
        }

        static float ComputeInnerProduct(const Vector2& lhs, const Vector2& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y);
        }

        // 内積
        float dot(const Vector2& rhs) const { return ComputeInnerProduct(*this, rhs); }

        // 加減算
        friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs) { return Add(lhs, rhs); }
        friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs) { return Subtract(lhs, rhs); }

        // スカラー積
        friend Vector2 operator*(float lhs, const Vector2& rhs) { return ScaleByScalar(rhs, lhs); }
        friend Vector2 operator*(const Vector2& lhs, float rhs) { return ScaleByScalar(lhs, rhs); }

        // スカラー商
        friend Vector2 operator/(const Vector2& lhs, float rhs) { return DivideByScalar(lhs, rhs); }

        // 要素ごとの積（アダマール積）
        friend Vector2 operator*(const Vector2& lhs, const Vector2& rhs) { return ComputeElementwiseProduct(lhs, rhs); }
    };

    struct Vector3
    {
        float x;
        float y;
        float z;

        static const Vector3 kZero;

        Vector3() = default;
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
        Vector3(const Vector2& v2, float z) : x(v2.x), y(v2.y), z(z) {}

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

        static Vector3 Normalize(const Vector3& v);

        static Vector3 Add(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        }

        static Vector3 Subtract(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        }

        static Vector3 ScaleByScalar(const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
        }

        static Vector3 DivideByScalar(const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector3 ComputeElementwiseProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        }

        static float ComputeInnerProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
        }

        static Vector3 ComputeOuterProduct(const Vector3& lhs, const Vector3& rhs)
        {
            return Vector3(
                (lhs.y * rhs.z) - (lhs.z * rhs.y),
                (lhs.z * rhs.x) - (lhs.x * rhs.z),
                (lhs.x * rhs.y) - (lhs.y * rhs.x)
            );
        }

        // 内積
        float dot(const Vector3& rhs) const { return ComputeInnerProduct(*this, rhs); }

        // 外積
        Vector3 cross(const Vector3& rhs) const { return ComputeOuterProduct(*this, rhs); }

        // スカラー積
        Vector3& operator*=(float rhs) { *this = ScaleByScalar(*this, rhs); return *this; }

        // スカラー商
        Vector3& operator/=(float rhs) { *this = DivideByScalar(*this, rhs); return *this; }

        // 加減算
        friend Vector3 operator+(const Vector3& lhs, const Vector3& rhs) { return Add(lhs, rhs); }
        friend Vector3 operator-(const Vector3& lhs, const Vector3& rhs) { return Subtract(lhs, rhs); }

        // スカラー積
        friend Vector3 operator*(float lhs, const Vector3& rhs) { return ScaleByScalar(rhs, lhs); }
        friend Vector3 operator*(const Vector3& lhs, float rhs) { return ScaleByScalar(lhs, rhs); }

        // スカラー商
        friend Vector3 operator/(const Vector3& lhs, float rhs) { return DivideByScalar(lhs, rhs); }

        // 要素ごとの積（アダマール積）
        friend Vector3 operator*(const Vector3& lhs, const Vector3& rhs) { return ComputeElementwiseProduct(lhs, rhs); }
    };

    struct Vector4
    {
        float x;
        float y;
        float z;
        float w;

        static const Vector4 kZero;

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

        float getComponent(int index) const;

        static Vector4 Add(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        }

        static Vector4 Subtract(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        }

        static Vector4 ScaleByScalar(const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
        }

        static Vector4 DivideByScalar(const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
        }

        // 要素ごとの積（アダマール積）
        static Vector4 ComputeElementwiseProduct(const Vector4& lhs, const Vector4& rhs)
        {
            return Vector4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        static float ComputeInnerProduct(const Vector4& lhs, const Vector4& rhs)
        {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
        }

        // 内積
        float dot(const Vector4& rhs) const { return ComputeInnerProduct(*this, rhs); }

        // スカラー積
        Vector4& operator*=(float rhs) { *this = ScaleByScalar(*this, rhs); return *this; }

        // スカラー商
        Vector4& operator/=(float rhs) { *this = DivideByScalar(*this, rhs); return *this; }

        // 加減算
        friend Vector4 operator+(const Vector4& lhs, const Vector4& rhs) { return Add(lhs, rhs); }
        friend Vector4 operator-(const Vector4& lhs, const Vector4& rhs) { return Subtract(lhs, rhs); }

        // スカラー積
        friend Vector4 operator*(float lhs, const Vector4& rhs) { return ScaleByScalar(rhs, lhs); }
        friend Vector4 operator*(const Vector4& lhs, float rhs) { return ScaleByScalar(lhs, rhs); }

        // スカラー商
        friend Vector4 operator/(const Vector4& lhs, float rhs) { return DivideByScalar(lhs, rhs); }

        // 要素ごとの積（アダマール積）
        friend Vector4 operator*(const Vector4& lhs, const Vector4& rhs) { return ComputeElementwiseProduct(lhs, rhs); }
    };
}
