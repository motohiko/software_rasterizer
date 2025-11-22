#pragma once

#include "..\Core\Types.h"
#include <cstdint>

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
