#pragma once

#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct VertexCacheEntry // = Vertex URB Entry.
    {
        VertexCacheEntry* prev;
        VertexCacheEntry* next;

        int vertexIndex;
        VertexDataA vertexDataA;// Pre
        VertexDataB vertexDataB;// Bufferd Vertex
    };

    class VertexCache
    {

    public:

        static void InitializeCache();

        static VertexCacheEntry* LookupVertexCache(int vertexIndex);
        static VertexCacheEntry* WriteVertexCache(int vertexIndex);

    };
}
