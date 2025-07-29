#pragma once

#include "Vector.h"
#include <cstdint>

namespace SoftwareRenderer
{
    const int kMaxVertexAttributes = 16;// GL_MAX_VERTEX_ATTRIBS
    const int kMaxVaryingVectors = 15;// GL_MAX_VARYING_VECTORS

    struct AssembledVertex
    {
        Vector4 attributes[kMaxVertexAttributes];
    };

    struct AssembledPrimitive
    {
        AssembledVertex vertices[3];
        int vertexNum;
    };

    struct Vertex
    {
        Vector4 clipSpacePosition;// clip space coordinates
        Vector4 varyingVariables[kMaxVaryingVectors];
        int varyingVariableNum;
    };

    struct Primitive
    {
        Vertex vertices[3];
        int vertexNum;
    };

    struct Fragment
    {
        Vector2 wrcPosition;// window relative coordinate
        float depth;
        float invW;
        Vector4 varyingVariables[kMaxVaryingVectors];

        bool helperInvocation;//  gl_helperinvocation
    };

    struct FragmentQuad
    {
        Fragment fragments[4];
    };

}
