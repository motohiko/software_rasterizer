#include "MeshData.h"

namespace Test
{
    // TODO GetMeshData 経由に変える

    const uint16_t kMeshTriangles[] =
    {
#include "LowPolyUnityChan\triangles.txt"
    };

    const int kMeshTrianglesLength = sizeof(kMeshTriangles) / sizeof(kMeshTriangles[0]);

    const float kMeshVertices[] =
    {
#include "LowPolyUnityChan\vertices.txt"
    };

    const int kMeshVerticesLength = sizeof(kMeshVertices) / sizeof(kMeshVertices[0]);

    const float kMeshNormals[] =
    {
#include "LowPolyUnityChan\normals.txt"
    };

    const int kMeshNormalsLength = sizeof(kMeshNormals) / sizeof(kMeshNormals[0]);

    const float kMeshUvs[] =
    {
#include "LowPolyUnityChan\uvs.txt"
    };

    const int kMeshUvsLength = sizeof(kMeshUvs) / sizeof(kMeshUvs[0]);

    const uint8_t kTexture[] =
    {
#include "LowPolyUnityChan\texture.txt"
    };

    const int kTextureLength = sizeof(kTexture) / sizeof(kTexture[0]);
}
