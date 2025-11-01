#include "PrimitiveAssembly.h"

namespace SoftwareRasterizer
{
    void PrimitiveAssembly::setPrimitiveType(PrimitiveType primitiveType)
    {
        _primitiveType = primitiveType;
    }

    void PrimitiveAssembly::setClipedVertices(const ShadedVertex* vertices, int vertiexNum)
    {
        _vertiexNum = vertiexNum;
    }

    void PrimitiveAssembly::prepareDividPrimitive()
    {
        _readPrimitiveCount = 0;
    }

    bool PrimitiveAssembly::readPrimitive(AssembledPrimitive* assembledPrimitive)
    {
        switch (_primitiveType)
        {
        case PrimitiveType::kLine:
            if ((0 == _readPrimitiveCount) && (2 == _vertiexNum))
            {
                assembledPrimitive->primitiveType = _primitiveType;
                assembledPrimitive->vertexIndices[0] = 0;
                assembledPrimitive->vertexIndices[1] = 1;
                assembledPrimitive->vertexNum = 2;
                _readPrimitiveCount++;
                return true;
            }
            break;
        case PrimitiveType::kTriangle:
            if ((_readPrimitiveCount + 2) < _vertiexNum)
            {
                assembledPrimitive->primitiveType = _primitiveType;
                assembledPrimitive->vertexIndices[0] = 0;
                assembledPrimitive->vertexIndices[1] = _readPrimitiveCount + 1;
                assembledPrimitive->vertexIndices[2] = _readPrimitiveCount + 2;
                assembledPrimitive->vertexNum = 3;
                _readPrimitiveCount++;
                return true;
            }
            break;
        default:
            break;
        }

        assembledPrimitive->primitiveType = PrimitiveType::kUndefined;
        assembledPrimitive->vertexNum = 0;
        return false;
    }
}
