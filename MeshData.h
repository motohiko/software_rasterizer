#pragma once

#include "Vector.h"
#include "Texture.h"
#include <cstdint>

namespace SoftwareRenderer
{
    namespace Test
    {
        extern const uint16_t kMeshTriangles[];
        extern const int kMeshTrianglesLength;
        extern const Vector3 kMeshVertices[];
        extern const int kMeshVerticesLength;
        extern const Vector3 kMeshNormals[];
        extern const int kMeshNormalsLength;
        extern const Vector2 kMeshUvs[];
        extern const int kMeshUvsLength;
        extern const Texel kTexture[];
        extern const int kTextureLength;
    }
}
