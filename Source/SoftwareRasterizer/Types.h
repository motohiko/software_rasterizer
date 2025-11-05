#pragma once

#include "..\Lib\Vector.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    enum class PrimitiveTopologyType
    {
        kUndefined,
        kLineList,
        kTriangleList,
    };

    enum class SemanticsType
    {
        kPosition,
        kNormal,
        kTexCoord,
        kColor,
    };

    enum class ComponentType
    {
        kFloat,// GL_FLOAT
        kUnsignedByte,// GL_UNSIGNED_BYTE
    };

    enum class FrontFaceType
    {
        kClockwise, //GL_CW
        kCounterClockwise,// GL_CCW
        kDefault = kCounterClockwise,
    };

    enum class CullFaceType
    {
        kNone,
        kFront, // GL_FRONT
        kBack, // GL_BACK
        kFrontAndBack, // GL_FRONT_AND_BACK
        kDefault = kBack,
    };

    const int kMaxVertexAttributes = 16;// GL_MAX_VERTEX_ATTRIBS
    const int kMaxVaryings = 15;// GL_MAX_VARYING_VECTORS

    enum class PrimitiveType
    {
        kUndefined,
        kLine,
        kTriangle,
    };

    struct AttributeVertex
    {
        Vector4 attributes[kMaxVertexAttributes];
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
