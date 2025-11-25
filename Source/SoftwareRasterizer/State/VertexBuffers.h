#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct VertexBuffer
    {
        const void* addr = nullptr;
    };

    struct VertexBuffers
    {
        VertexBuffer vertexBuffers[kMaxVertexAttributes] = {};
    };

}
