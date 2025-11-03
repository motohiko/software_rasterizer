#pragma once

#include "Lib\Vector.h"
#include <cstdint>

namespace SoftwareRasterizer
{
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
        Vector4 clipSpacePosition;
        Vector4 varyings[kMaxVaryings];
        int varyingNum;
    };

    struct Fragment
    {
        int x;
        int y;

        Vector2 wrcPosition;// window relative coordinate
        float depth;
        float invW;// 1 / clip space position w
        Vector4 varyings[kMaxVaryings];
        int varyingNum;
    };
}
