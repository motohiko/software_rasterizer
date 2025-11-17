#include "RenderingContext.h"
#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\ClipStage.h"
#include "Pipeline\PrimitiveAssembly.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "..\Lib\Algorithm.h"
#include <iterator>// std::size
#include <algorithm>// fill
#include <cassert>

namespace SoftwareRasterizer
{
    RenderingContext::RenderingContext()
    {
    }

    void RenderingContext::setWindowSize(int width, int height)
    {
        _windowWidth = width;
        _windowHeight = height;
    }

    int RenderingContext::getWindowWidth() const
    {
        return _windowWidth;
    };

    int RenderingContext::getWindowHeight() const
    {
        return _windowHeight;
    };

    void RenderingContext::setRenderTargetColorBuffer(void* addr, int width, int height, size_t widthBytes)
    {
        // OM
        _renderTarget.colorBuffer.addr = addr;
        _renderTarget.colorBuffer.width = width;
        _renderTarget.colorBuffer.height = height;
        _renderTarget.colorBuffer.widthBytes = widthBytes;
    }

    void RenderingContext::setRenderTargetDepthBuffer(void* addr, int width, int height, size_t widthBytes)
    {
        // OM
        _renderTarget.depthBuffer.addr = addr;
        _renderTarget.depthBuffer.width = width;
        _renderTarget.depthBuffer.height = height;
        _renderTarget.depthBuffer.widthBytes = widthBytes;
    }

    void RenderingContext::setClearColor(float r, float g, float b, float a)
    {
        // Non pipeline operation
        _clearColor = Vector4(r, g, b, a);
    }

    void RenderingContext::setClearDepth(float depth)
    {
        // Non pipeline operation
        _clearDepth = depth;
    }

    void RenderingContext::clearRenderTarget()
    {
        // Non pipeline operation
        clearRenderTargetColorBuffer();
        clearRenderTargetDepthBuffer();
    }

    void RenderingContext::enableVertexAttribute(int index)
    {
        // IA
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        _inputLayout.vertexAttributeEnableBits |= (1u << index);
    }

    void RenderingContext::disableVertexAttribute(int index)
    {
        // IA
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        _inputLayout.vertexAttributeEnableBits &= ~(1u << index);
    }

    void RenderingContext::setVertexAttribute(int index, SemanticsType semantics, int size, ComponentType type, size_t stride, const void* buffer)
    {
        // IA
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        InputElement* element = &(_inputLayout.elements[index]);
        element->semantics = semantics;
        element->size = size;
        element->type = type;
        element->normalized = false;
        element->stride = stride;
        VertexBuffer* vertexBuffer = &(_vertexBuffers.vertexBuffers[index]);
        vertexBuffer->addr = buffer;
    }

    void RenderingContext::setIndexBuffer(const uint16_t* indices, int indexNum)
    {
        // IA
        _indexBuffer.indices = indices;
        _indexBuffer.indexNum = indexNum;
    }

    void RenderingContext::setUniformBlock(const void* uniformBlock)
    {
        // VS / PS
        _constantBuffer.uniformBlock = uniformBlock;
    }

    void RenderingContext::setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain)
    {
        // VS
        _vertexShaderProgram.vertexShaderMain = vertexShaderMain;
    }

    void RenderingContext::setViewport(int x, int y, int width, int height)
    {
        // RS
        _viewport.x = x;
        _viewport.y = y;
        _viewport.width = width;
        _viewport.height = height;
    }

    int RenderingContext::getViewportWidth() const
    {
        return _viewport.width;
    }

    int RenderingContext::getViewportHeight() const
    {
        return _viewport.height;
    }

    void RenderingContext::setDepthRange(float nearVal, float farVal)
    {
        // RS
        _viewport.depthRangeNearVal = nearVal;
        _viewport.depthRangeFarVal = farVal;
    }

    void RenderingContext::setFrontFaceType(FrontFaceType frontFacetype)
    {
        // RS
        _rasterizerState.frontFacetype = frontFacetype;
    }

    void RenderingContext::setCullFaceType(CullFaceType cullFaceType)
    {
        // RS
        _rasterizerState.cullFaceType = cullFaceType;
    }

    void RenderingContext::setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain)
    {
        // PS
        _fragmentShaderProgram.fragmentShaderMain = fragmentShaderMain;
    }

    void RenderingContext::setDepthFunc(ComparisonType depthFunc)
    {
        // OM
        _depthState.depthFunc = depthFunc;
    }

    void RenderingContext::drawIndexed(PrimitiveTopologyType primitiveTopologyType)
    {
        // IA I/O
        _inputAssemblyStage.input(&_inputLayout);
        _inputAssemblyStage.input(&_vertexBuffers);
        _inputAssemblyStage.input(&_indexBuffer);
        _inputAssemblyStage.input(primitiveTopologyType);

        // VS I/O
        _vertexShaderStage.input(&_constantBuffer);
        _vertexShaderStage.input(&_vertexShaderProgram);

        // RS I/O
        _rasterizeStage.setWindowSize(_windowWidth, _windowHeight);
        _rasterizeStage.input(&_rasterizerState);
        _rasterizeStage.input(&_viewport);
        _rasterizeStage.ouput(this);

        // PS I/O
        _fragmentShaderStage.input(&_constantBuffer);
        _fragmentShaderStage.input(&_fragmentShaderProgram);

        // OM I/O
        _outputMergerStage.input(&_renderTarget);
        _outputMergerStage.input(&_depthState);
        _outputMergerStage.ouput(this);


        _inputAssemblyStage.prepareReadPrimitive();
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

    void RenderingContext::clearRenderTargetColorBuffer()
    {
        assert(nullptr != _renderTarget.colorBuffer.addr);
        assert(0 <= _renderTarget.colorBuffer.width);
        assert(0 <= _renderTarget.colorBuffer.height);
        assert(_renderTarget.colorBuffer.width <= _renderTarget.colorBuffer.widthBytes);

        float r = _clearColor.x;
        float g = _clearColor.y;
        float b = _clearColor.z;
        float a = _clearColor.w;
        uint32_t color = (Lib::DenormalizeByte(a) << 24) | (Lib::DenormalizeByte(r) << 16) | (Lib::DenormalizeByte(g) << 8) | Lib::DenormalizeByte(b);

        // 
        uint32_t* first = (uint32_t*)_renderTarget.colorBuffer.addr;
        uint32_t* last = ((uint32_t*)_renderTarget.colorBuffer.addr) + _renderTarget.colorBuffer.width;
        std::fill(first, last, color);

        // 
        uintptr_t addr = (uintptr_t)(_renderTarget.colorBuffer.addr);
        size_t height = _renderTarget.colorBuffer.height;
        size_t widthBytes = _renderTarget.colorBuffer.widthBytes;
        for (int y = 1; y < height; y++)
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + (widthBytes * y);
            std::memcpy((void*)dst, (const void*)src, widthBytes);
        }
    }

    void RenderingContext::clearRenderTargetDepthBuffer()
    {
        assert(nullptr != _renderTarget.depthBuffer.addr);
        assert(0 <= _renderTarget.depthBuffer.width);
        assert(0 <= _renderTarget.depthBuffer.height);
        assert(_renderTarget.depthBuffer.width <= _renderTarget.depthBuffer.widthBytes);

        // 
        float* first = (float*)_renderTarget.depthBuffer.addr;
        float* last = ((float*)_renderTarget.depthBuffer.addr) + (_renderTarget.depthBuffer.width);
        std::fill(first, last, _clearDepth);

        // 
        uintptr_t addr = (uintptr_t)(_renderTarget.depthBuffer.addr);
        size_t height = _renderTarget.depthBuffer.height;
        size_t widthBytes = _renderTarget.depthBuffer.widthBytes;
        for (int y = 1; y < height; y++)
        {
            uintptr_t src = addr;
            uintptr_t dst = addr + (widthBytes * y);
            std::memcpy((void*)dst, (const void*)src, widthBytes);
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

        _outputMergerStage.execute(fragment->x, fragment->y, color, fragment->depth);
    }

}
