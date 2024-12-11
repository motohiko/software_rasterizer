#include "MeshData.h"

#include "Matrix.h"

namespace MyApp
{
    // TODO float, uint8_t の配列に変える

    const uint16_t kMeshTriangles[] =
    {
#include "LowPolyUnityChan\triangles.txt"
    };

    const int kMeshTrianglesLength = sizeof(kMeshTriangles) / sizeof(kMeshTriangles[0]);

    const Vector3 kMeshVertices[] =
    {
#include "LowPolyUnityChan\vertices.txt"
    };

    const int kMeshVerticesLength = sizeof(kMeshVertices) / sizeof(kMeshVertices[0]);

    const Vector3 kMeshNormals[] =
    {
#include "LowPolyUnityChan\normals.txt"
    };

    const int kMeshNormalsLength = sizeof(kMeshNormals) / sizeof(kMeshNormals[0]);

    const Vector2 kMeshUvs[] =
    {
#include "LowPolyUnityChan\uvs.txt"
    };

    const int kMeshUvsLength = sizeof(kMeshUvs) / sizeof(kMeshUvs[0]);

    const Texel kTexture[] =
    {
#include "LowPolyUnityChan\texture.txt"
    };

    const int kTextureLength = sizeof(kTexture) / sizeof(kTexture[0]);
};
