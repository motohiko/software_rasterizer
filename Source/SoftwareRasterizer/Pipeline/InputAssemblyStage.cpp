#include "InputAssemblyStage.h"
#include "..\RenderingContext.h"
#include "..\Modules\VertexFetchUnit.h"
#include "..\Modules\VertexCache.h"

namespace SoftwareRasterizer
{
    void InputAssemblyStage::validateState(const InputLayout* state)
    {
    }

    InputAssemblyStage::InputAssemblyStage()
    {
    }

    InputAssemblyStage::~InputAssemblyStage()
    {
    }

    void InputAssemblyStage::prepareReadPrimitive()
    {
        switch (_primitiveTopologyType)
        {
        case PrimitiveTopologyType::kLineList:
            _primitiveType = PrimitiveType::kLine;
            _primitiveVertexNum = 2;
            break;
        case PrimitiveTopologyType::kTriangleList:
            _primitiveType = PrimitiveType::kTriangle;
            _primitiveVertexNum = 3;
            break;
        default:
            _primitiveType = PrimitiveType::kNone;
            _primitiveVertexNum = 0;
            break;
        }

        _readVertexCount = 0;
    }

    void InputAssemblyStage::executeVertexLoop()
    {
        Primitive primitive;
        while (readPrimitive(&primitive))
        {
            int vertexNum = primitive.vertexNum;
            uint16_t* vertexIndices = primitive.vertexIndices;

            VertexDataB* clipVertices[3];

            for (int i = 0; i < vertexNum; i++)
            {
                // gen vertex id.
                int vertexIndex = vertexIndices[i];

                // cache lookup
                VertexCacheEntry* entry = VertexCache::LookupVertexCache(vertexIndex);
                bool miss = (nullptr == entry);
                if (miss)
                {
                    entry = VertexCache::WriteVertexCache(vertexIndex);

                    VertexDataA* vertex = &(entry->vertexDataA);
                    VertexFetchUnit::FetchVertex(_inputLayout, _vertexBuffers, vertexIndex, vertex);

                    _renderingContext->outputVertex(entry);
                }

                clipVertices[i] = &(entry->vertexDataB);
            }

            _renderingContext->outputPrimitive(primitive.primitiveType, clipVertices, primitive.vertexNum);
        }
    }

    bool InputAssemblyStage::readPrimitive(InputAssemblyStage::Primitive* primitive)
    {
        int remainingVertexCount = _indexBuffer->indexNum - _readVertexCount;
        if (remainingVertexCount < _primitiveVertexNum)
        {
            primitive->primitiveType = PrimitiveType::kNone;
            primitive->vertexNum = 0;
            return false;
        }

        for (int i = 0; i < _primitiveVertexNum; i++)
        {
            uint16_t vertexIndex = _indexBuffer->indices[_readVertexCount];
            _readVertexCount++;

            primitive->vertexIndices[i] = vertexIndex;
        }

        primitive->primitiveType = _primitiveType;
        primitive->vertexNum = _primitiveVertexNum;

        return true;
    }

}
