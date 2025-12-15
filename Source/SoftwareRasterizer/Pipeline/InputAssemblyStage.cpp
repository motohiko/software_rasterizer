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
        for (;;)
        {
            int remainingVertexCount = _indexBuffer->indexNum - _readVertexCount;
            if (remainingVertexCount < _primitiveVertexNum)
            {
                return;
            }

            uint16_t vertexIndices[3];
            for (int i = 0; i < _primitiveVertexNum; i++)
            {
                uint16_t vertexIndex = _indexBuffer->indices[_readVertexCount];
                _readVertexCount++;
                vertexIndices[i] = vertexIndex;
            }

            VertexCacheEntry* entries[3];
            for (int i = 0; i < _primitiveVertexNum; i++)
            {
                int vertexIndex = vertexIndices[i];

                // gen vertex id.
                int vertexId = vertexIndex;

                // cache lookup
                VertexCacheEntry* entry = VertexCache::LookupVertexCache(vertexId);
                bool miss = (nullptr == entry);
                if (miss)
                {
                    entry = VertexCache::GetVertexCache(vertexId);

                    VertexDataA* vertex = &(entry->vertexDataA);// Pre T/L
                    vertex->vertexId = vertexId;
                    VertexFetchUnit::FetchVertex(_inputLayout, _vertexBuffers, vertexIndex, vertex);

                    _renderingContext->outputVertex(entry);
                }

                entries[i] = entry;
            }

            _renderingContext->outputPrimitive(_primitiveType, entries, _primitiveVertexNum);
        }
    }

}
