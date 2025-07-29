#include "InputAssemblyStage.h"
#include "Algorithm.h"
#include <cassert>

namespace SoftwareRenderer
{
    void InputAssemblyStage::setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType)
    {
        switch (primitiveTopologyType)
        {
        case PrimitiveTopologyType::LineList:
            _primitiveVertexNum = 2;
            break;
        case PrimitiveTopologyType::TriangleList:
            _primitiveVertexNum = 3;
            break;
        default:
            _primitiveVertexNum = 0;
            break;
        }
    }

    void InputAssemblyStage::enableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_vertexAttributeLayouts));
        _vertexAttributeEnableBits |= (1u << index);
    }

    void InputAssemblyStage::disableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_vertexAttributeLayouts));
        _vertexAttributeEnableBits &= ~(1u << index);
    }

    void InputAssemblyStage::setVertexBuffer(int index, const void* buffer)
    {
        assert(0 <= index && index < std::size(_vertexAttributeLayouts));
        InputVertexAttributeLayout* attr = &(_vertexAttributeLayouts[index]);
        attr->buffer = buffer;
    }

    void InputAssemblyStage::setVertexAttribute(int index, Semantics semantics, int size, ComponentType type, size_t stride)
    {
        assert(0 <= index && index < std::size(_vertexAttributeLayouts));
        InputVertexAttributeLayout* attr = &(_vertexAttributeLayouts[index]);
        attr->semantics = semantics;
        attr->size = size;
        attr->type = type;
        attr->normalized = false;
        attr->stride = stride;
    }

    void InputAssemblyStage::setIndexBuffer(const uint16_t* indices, int indexNum)
    {
        _indexBuffer.indices = indices;
        _indexBuffer.indexNum = indexNum;
    }

    void InputAssemblyStage::resetReadCount()
    {
        _readVertexCount = 0;

        // TODO
        _vertexAttributeNum = 0;
        for (int i = sizeof(_vertexAttributeEnableBits) - 1; 0 <= i; i--)
        {
            if (_vertexAttributeEnableBits & (1u << i))
            {
                _vertexAttributeNum = 1 + i;
                break;
            }
        }
    }

    bool InputAssemblyStage::readPrimitive(AssembledPrimitive* assembledPrimitive)
    {
        int remainingVertexCount = _indexBuffer.indexNum - _readVertexCount;
        if (remainingVertexCount < _primitiveVertexNum)
        {
            assembledPrimitive->vertexNum = 0;
            return false;
        }

        assert(_primitiveVertexNum <= std::size(assembledPrimitive->vertices));
        for (int i = 0; i < _primitiveVertexNum; i++)
        {
            AssembledVertex* assembledVertex = &(assembledPrimitive->vertices[i]);

            assert(_readVertexCount < _indexBuffer.indexNum);
            uint16_t vertexIndex = _indexBuffer.indices[_readVertexCount];

            _readVertexCount++;

            assert(_vertexAttributeNum <= std::size(assembledVertex->attributes));
            assert(_vertexAttributeNum <= std::size(_vertexAttributeLayouts));
            for (int j = 0; j < _vertexAttributeNum; j++)
            {
                if (_vertexAttributeEnableBits & (1u << j))
                {
                    assembledVertex->attributes[j] = readVertexAttribute(&(_vertexAttributeLayouts[j]), vertexIndex);
                }
            }
        }

        assembledPrimitive->vertexNum = _primitiveVertexNum;

        return true;
    }

    Vector4 InputAssemblyStage::readVertexAttribute(const InputVertexAttributeLayout* vertexAttributeLayout, uint16_t vertexIndex) const
    {
        Vector4 ret;

        // セマンティクスごとのデフォルト値を代入しておく
        switch (vertexAttributeLayout->semantics)
        {
        case Semantics::Position:
            ret = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        default:
            ret = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            break;
        }

        // 指定個数のコンポーネントを読み取る（上書く）
        assert(vertexAttributeLayout->buffer);
        uintptr_t ptr = ((uintptr_t)vertexAttributeLayout->buffer) + (vertexAttributeLayout->stride * vertexIndex);
        switch (vertexAttributeLayout->type)
        {
        case ComponentType::Float:
            switch (vertexAttributeLayout->size)
            {
            case 4: ret.w = ((const float*)ptr)[3];
            case 3: ret.z = ((const float*)ptr)[2];
            case 2: ret.y = ((const float*)ptr)[1];
            case 1: ret.x = ((const float*)ptr)[0];
            default: break;
            }
            break;
        case ComponentType::UnsignedByte:
            switch (vertexAttributeLayout->size)
            {
            case 4: ret.w = ((const uint8_t*)ptr)[3];
            case 3: ret.z = ((const uint8_t*)ptr)[2];
            case 2: ret.y = ((const uint8_t*)ptr)[1];
            case 1: ret.x = ((const uint8_t*)ptr)[0];
            default: break;
            }
            break;
        default:
            assert(false);
            break;
        }

        return ret;
    }
}
