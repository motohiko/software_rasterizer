#include "RenderingContext.h"
#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\ClipStage.h"
#include "Pipeline\PrimitiveAssembly.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include <iterator>// std::size
#include <cassert>

namespace SoftwareRasterizer
{
    RenderingContext::RenderingContext() :
        _inputAssemblyStage(this),
        _vertexShaderStage(this),
        _rasterizeStage(this),
        _fragmentShaderStage(this)
    {
    }

    void RenderingContext::setFrameSize(int width, int height)
    {
        _frameBuffer.setFrameSize(width, height);
    }

    int RenderingContext::getFrameWidth() const
    {
        return _frameBuffer.getFrameWidth();
    };

    int RenderingContext::getFrameHeight() const
    {
        return _frameBuffer.getFrameHeight();
    };

    void RenderingContext::setFrameColorBuffer(void* addr, size_t widthBytes)
    {
        _frameBuffer.setColorBuffer(addr, widthBytes);
    }

    void RenderingContext::setFrameDepthBuffer(void* addr, size_t widthBytes)
    {
        _frameBuffer.setDepthBuffer(addr, widthBytes);
    }

    void RenderingContext::setClearColor(float r, float g, float b, float a)
    {
        _frameBuffer.setClearColor(r, g, b, a);
    }

    void RenderingContext::setClearDepth(float depth)
    {
        _frameBuffer.setClearDepth(depth);
    }

    void RenderingContext::clearFrameBuffer()
    {
        _frameBuffer.clearBuffer();
    }

    void RenderingContext::enableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        _inputAssemblyStageState.vertexAttributeEnableBits |= (1u << index);
    }

    void RenderingContext::disableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        _inputAssemblyStageState.vertexAttributeEnableBits &= ~(1u << index);
    }

    void RenderingContext::setVertexBuffer(int index, const void* buffer)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        VertexAttributeLayout* attribute = &(_inputAssemblyStageState.vertexAttributeLayouts[index]);
        attribute->buffer = buffer;
    }

    void RenderingContext::setVertexAttribute(int index, SemanticsType semantics, int size, ComponentType type, size_t stride)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        VertexAttributeLayout* attribute = &(_inputAssemblyStageState.vertexAttributeLayouts[index]);
        attribute->semantics = semantics;
        attribute->size = size;
        attribute->type = type;
        attribute->normalized = false;
        attribute->stride = stride;
    }

    void RenderingContext::setIndexBuffer(const uint16_t* indices, int indexNum)
    {
        _inputAssemblyStageState.indexBuffer.indices = indices;
        _inputAssemblyStageState.indexBuffer.indexNum = indexNum;
    }

    void RenderingContext::setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType)
    {
        _inputAssemblyStageState.primitiveTopologyType = primitiveTopologyType;
    }

    void RenderingContext::setUniformBlock(const void* uniformBlock)
    {
        _vertexShaderStageState.uniformBlock = uniformBlock;
        _fragmentShaderStageState.uniformBlock = uniformBlock;
    }

    void RenderingContext::setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain)
    {
        _vertexShaderStageState.vertexShaderMain = vertexShaderMain;
    }

    void RenderingContext::setViewport(int x, int y, int width, int height)
    {
        _rasterizeStageState.viewportX = x;
        _rasterizeStageState.viewportY = y;
        _rasterizeStageState.viewportWidth = width;
        _rasterizeStageState.viewportHeight = height;
    }

    int RenderingContext::getViewportWidth() const
    {
        return _rasterizeStageState.viewportWidth;
    }

    int RenderingContext::getViewportHeight() const
    {
        return _rasterizeStageState.viewportHeight;
    }

    void RenderingContext::setDepthRange(float nearVal, float farVal)
    {
        _rasterizeStageState.depthRangeNearVal = nearVal;
        _rasterizeStageState.depthRangeFarVal = farVal;
    }

    void RenderingContext::setFrontFaceType(FrontFaceType frontFacetype)
    {
        _rasterizeStageState.frontFacetype = frontFacetype;
    }

    void RenderingContext::setCullFaceType(CullFaceType cullFaceType)
    {
        _rasterizeStageState.cullFaceType = cullFaceType;
    }

    void RenderingContext::setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain)
    {
        _fragmentShaderStageState.fragmentShaderMain = fragmentShaderMain;
    }

    void RenderingContext::setDepthFunc(ComparisonType depthFunc)
    {
        _depthFunc = depthFunc;
    }

    void RenderingContext::drawIndexed()
    {
        InputAssemblyStage::validateState(&_inputAssemblyStageState);
        VertexShaderStage::validateState(&_vertexShaderStageState);
        RasterizeStage::validateState(&_rasterizeStageState);
        FragmentShaderStage::validateState(&_fragmentShaderStageState);

        _inputAssemblyStage.prepareReadPrimitive();
        _inputAssemblyStage.prepareReadVertex();

        _rasterizeStage.setFrameSize(_frameBuffer.getFrameWidth(), _frameBuffer.getFrameHeight());
        _rasterizeStage.prepareRasterize();

        InputAssemblyStage::Primitive primitive;
        while (_inputAssemblyStage.readPrimitive(&primitive))
        {
            ShadedVertex shadedVertices[3];

            for (int i = 0; i < primitive.vertexNum; i++)
            {
                uint16_t vertexIndex = primitive.vertexIndices[i];

                AttributeVertex attributeVertex;
                _inputAssemblyStage.readAttributeVertex(vertexIndex, &attributeVertex);

                _vertexShaderStage.executeShader(&attributeVertex, &(shadedVertices[i]));
            }

            outputPrimitive(primitive.primitiveType, shadedVertices, primitive.vertexNum);
        }
    }

    void RenderingContext::outputPrimitive(PrimitiveType primitiveType, const ShadedVertex* vertices, int vertexNum)
    {
        // プリミティブをクリップ
        ShadedVertex clippedVertices[kClippingPointMaxNum];
        int clippedVertiexNum = 0;
        {
            ClipStage clipStage;
            clipStage.setPrimitiveType(primitiveType);
            clipStage.clipPrimitive(vertices, vertexNum, clippedVertices, &clippedVertiexNum);
        }

        // クリップ結果をプリミティブに分割
        PrimitiveAssembly primitiveAssembly;
        primitiveAssembly.setPrimitiveType(primitiveType);
        primitiveAssembly.setClipedVertices(clippedVertices, clippedVertiexNum);
        primitiveAssembly.prepareDividPrimitive();
        AssembledPrimitive dividedPrimitive;
        while (primitiveAssembly.readPrimitive(&dividedPrimitive))
        {
            // （分割された）各プリミティブをラスタライズ

            RasterPrimitive rasterPrimitive;
            rasterPrimitive.primitiveType = dividedPrimitive.primitiveType;
            for (int i = 0; i < dividedPrimitive.vertexNum; i++)
            {
                uint16_t vertexIndex = dividedPrimitive.vertexIndices[i];
                rasterPrimitive.vertices[i] = clippedVertices[vertexIndex];
            }
            rasterPrimitive.vertexNum = dividedPrimitive.vertexNum;

            _rasterizeStage.rasterizePrimitive(rasterPrimitive);
        }
    }

    void RenderingContext::outputFragment(const Fragment* fragment)
    {
        Vector4 color;
        _fragmentShaderStage.executeShader(fragment, &color);

        bool passed = depthTest(fragment->x, fragment->y, fragment->depth);
        if (!passed)
        {
            return;
        }

        _frameBuffer.writePixel(fragment->x, fragment->y, color, fragment->depth);
    }

    bool RenderingContext::depthTest(int x, int y, float depth)
    {
        float storedDepth = _frameBuffer.readDepth(x, y);

        bool passed;
        switch (_depthFunc)
        {
        case ComparisonType::kNever:
            passed = false;
            break;
        case ComparisonType::kLess:
            passed = (depth < storedDepth);
            break;
        case ComparisonType::kEqual:
            passed = (depth == storedDepth);
            break;
        case ComparisonType::kLessEqual:
            passed = (depth <= storedDepth);
            break;
        case ComparisonType::kGreater:
            passed = (depth > storedDepth);
            break;
        case ComparisonType::kNotEqual:
            passed = (depth != storedDepth);
            break;
        case ComparisonType::kGreaterEqual:
            passed = (depth >= storedDepth);
            break;
        case ComparisonType::kAlways:
            passed = true;
            break;
        default:
            passed = false;
            break;
        }
        
        return passed;
    }

}
