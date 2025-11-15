#pragma once

#include "..\Lib\Vector.h"
#include "..\Lib\Matrix.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    using Lib::Vector2;
    using Lib::Vector3;
    using Lib::Vector4;
    using Lib::Matrix4x4;

    enum class PrimitiveTopologyType
    {
        kNone,
        kLineList,      // GL_LINES
        kTriangleList,  // GL_TRIANGLES
    };

    enum class SemanticsType
    {
        kNone,
        kPosition,
        kNormal,
        kTexCoord,
        kColor,
    };

    enum class ComponentType
    {
        kNone,
        kFloat,         // GL_FLOAT
        kUnsignedByte,  // GL_UNSIGNED_BYTE
    };

    enum class FrontFaceType
    {
        kNone,
        kClockwise,         //GL_CW
        kCounterClockwise,  // GL_CCW
        kDefault = kCounterClockwise,
    };

    enum class CullFaceType
    {
        kNone,
        kFront,         // GL_FRONT
        kBack,          // GL_BACK
        kFrontAndBack,  // GL_FRONT_AND_BACK
        kDefault = kBack,
    };

    enum class ComparisonType
    {
        kNone,
        kNever,         // GL_NEVER
        kLess,          // GL_LESS
        kEqual,         // GL_EQUAL
        kLessEqual,     // GL_LEQUAL
        kGreater,       // GL_GREATER
        kNotEqual,      // GL_NOTEQUAL
        kGreaterEqual,  // GL_GEQUAL
        kAlways,        // GL_ALWAYS
        kDefault = kLess,
    };

    enum class PrimitiveType
    {
        kNone,
        kLine,
        kTriangle,
    };

    const int kMaxVertexAttributes = 16;// GL_MAX_VERTEX_ATTRIBS
    const int kMaxVaryings = 15;        // GL_MAX_VARYING_VECTORS

    struct AttributeVertex
    {
        Vector4 attributes[kMaxVertexAttributes];
        uint32_t attributeEnableBits;
    };

    struct ShadedVertex
    {
        Vector4 clipPosition;// 頂点座標（クリップ空間座標系）
        Vector4 varyings[kMaxVaryings];
        int varyingNum;
    };

    struct NdcVertex
    {
        Vector3 ndcPosition;// 頂点座標（正規化デバイス座標系）
    };

    struct RasterVertex
    {
        Vector2 wndPosition;// 頂点座標（ウィンドウ座標系）
        float depth;// 深度
        float invW;// = 1 / clipPosition.w

        Vector4 varyingsDividedByW[kMaxVaryings];
        int varyingNum;
    };

    struct Fragment
    {
        int x, y;// フラグメントの座標（ウィンドウ座標系）

        Vector2 wndPosition;// フラグメントの中心座標（ウィンドウ座標系）
        float depth;// 深度
        float invW;// = 1 / clipPosition.w

        Vector4 varyings[kMaxVaryings];
        int varyingNum;
    };
}
