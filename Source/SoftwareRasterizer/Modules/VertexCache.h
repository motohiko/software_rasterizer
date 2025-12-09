#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct VertexCacheEntry // = Vertex URB Entry.
    {
        VertexCacheEntry* prev;
        VertexCacheEntry* next;

        bool enabled;
        int vertexId;

        VertexDataA vertexDataA;// Pre T/L
        VertexDataB vertexDataB;// Post T/L
    };

    class VertexCache
    {

    public:

        static void InitializeCache();

        static VertexCacheEntry* LookupVertexCache(int vertexId);
        static VertexCacheEntry* GetVertexCache(int vertexId);

    };
}
