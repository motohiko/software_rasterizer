#include "RenderingContext.h"
#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Modules\ClipStage.h"
#include "Modules\PrimitiveAssembly.h"
#include "Modules\TextureOperations.h" 
#include <iterator>// std::size
#include <algorithm>// clamp
#include <cassert>

namespace SoftwareRasterizer
{
    RenderingContext::RenderingContext()
    {
    }

    void RenderingContext::setWindowSize(int width, int height)
    {
        _windowSize.windowWidth = width;
        _windowSize.windowHeight = height;
    }

    int RenderingContext::getWindowWidth() const
    {
        return _windowSize.windowWidth;
    };

    int RenderingContext::getWindowHeight() const
    {
        return _windowSize.windowHeight;
    };

    void RenderingContext::setRenderTargetColorBuffer(void* addr, int width, int height, int widthBytes)
    {
        //assert(nullptr != addr);
        //assert(0 <= width);
        //assert(0 <= height);
        //assert(width <= widthBytes);
        _renderTarget.colorBuffer.addr = addr;
        _renderTarget.colorBuffer.width = width;
        _renderTarget.colorBuffer.height = height;
        _renderTarget.colorBuffer.widthBytes = widthBytes;
    }

    void RenderingContext::setRenderTargetDepthBuffer(void* addr, int width, int height, int widthBytes)
    {
        //assert(nullptr != addr);
        //assert(0 <= width);
        //assert(0 <= height);
        //assert(width <= widthBytes);
        _renderTarget.depthBuffer.addr = addr;
        _renderTarget.depthBuffer.width = width;
        _renderTarget.depthBuffer.height = height;
        _renderTarget.depthBuffer.widthBytes = widthBytes;
    }

    void RenderingContext::setClearColor(float red, float green, float blue, float alpha)
    {
        _clearParam.clearColorR = std::clamp(red, 0.0f, 1.0f);
        _clearParam.clearColorG = std::clamp(green, 0.0f, 1.0f);
        _clearParam.clearColorB = std::clamp(blue, 0.0f, 1.0f);
        _clearParam.clearColorA = std::clamp(alpha, 0.0f, 1.0f);
    }

    void RenderingContext::setClearDepth(float depth)
    {
        _clearParam.clearDepth = std::clamp(depth, 0.0f, 1.0f);// saturate
    }

    void RenderingContext::clearRenderTarget()
    {
        Vector4 color(
            _clearParam.clearColorR,
            _clearParam.clearColorG,
            _clearParam.clearColorB,
            _clearParam.clearColorA
        );
        float depth = _clearParam.clearDepth;
        TextureOperations::FillTextureColor(&(_renderTarget.colorBuffer), color);
        TextureOperations::FillTextureDepth(&(_renderTarget.depthBuffer), depth);
    }

    void RenderingContext::enableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        _inputLayout.vertexAttributeEnableBits |= (1u << index);
    }

    void RenderingContext::disableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        _inputLayout.vertexAttributeEnableBits &= ~(1u << index);
    }

    void RenderingContext::setVertexAttribute(int index, int size, ComponentDataType type, size_t stride, const void* buffer)
    {
        assert(0 <= index && index < std::size(_inputLayout.elements));
        assert(0 <= index && index < std::size(_vertexBuffers.vertexBuffers));
        InputElement* element = &(_inputLayout.elements[index]);
        element->size = size;
        element->type = type;
        element->normalized = false;
        element->stride = stride;
        VertexBuffer* vertexBuffer = &(_vertexBuffers.vertexBuffers[index]);
        vertexBuffer->addr = buffer;
    }

    void RenderingContext::setIndexBuffer(const uint16_t* indices, int indexNum)
    {
        _indexBuffer.indices = indices;
        _indexBuffer.indexNum = indexNum;
    }

    void RenderingContext::setUniformBlock(const void* uniformBlock)
    {
        _constantBuffer.uniformBlock = uniformBlock;
    }

    void RenderingContext::setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain)
    {
        _vertexShaderProgram.vertexShaderMain = vertexShaderMain;
    }

    void RenderingContext::setViewport(int x, int y, int width, int height)
    {
        _viewport.viewportX = x;
        _viewport.viewportY = y;
        _viewport.viewportWidth = width;
        _viewport.viewportHeight = height;
    }

    int RenderingContext::getViewportWidth() const
    {
        return _viewport.viewportWidth;
    }

    int RenderingContext::getViewportHeight() const
    {
        return _viewport.viewportHeight;
    }

    void RenderingContext::setDepthRange(float nearVal, float farVal)
    {
        _depthRange.depthRangeNearVal = nearVal;
        _depthRange.depthRangeFarVal = farVal;
    }

    void RenderingContext::setFrontFaceMode(FrontFaceMode frontFaceMode)
    {
        _rasterizerState.frontFaceMode = frontFaceMode;
    }

    void RenderingContext::setCullFaceMode(CullFaceMode cullFaceMode)
    {
        _rasterizerState.cullFaceMode = cullFaceMode;
    }

    void RenderingContext::setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain)
    {
        _fragmentShaderProgram.fragmentShaderMain = fragmentShaderMain;
    }

    void RenderingContext::setDepthFunc(ComparisonFuncType depthFunc)
    {
        _depthState.depthFunc = depthFunc;
    }

    void RenderingContext::drawIndexed(PrimitiveTopologyType primitiveTopologyType)
    {
        _quadFragment.setQ00(&_q00);
        _quadFragment.setQ01(&_q01);
        _quadFragment.setQ10(&_q10);
        _quadFragment.setQ11(&_q11);

        // Set IA I/O.
        _inputAssemblyStage.input(&_inputLayout);
        _inputAssemblyStage.input(&_vertexBuffers);
        _inputAssemblyStage.input(&_indexBuffer);
        _inputAssemblyStage.input(primitiveTopologyType);

        // Set VS I/O.
        _vertexShaderStage.input(&_constantBuffer);
        _vertexShaderStage.input(&_vertexShaderProgram);

        // Set RS I/O.
        _rasterizeStage.input(&_windowSize);
        _rasterizeStage.input(&_rasterizerState);
        _rasterizeStage.input(&_viewport);
        _rasterizeStage.input(&_depthRange);
        _rasterizeStage.output(&_quadFragment);
        _rasterizeStage.output(this);

        // Set PS I/O.
        _fragmentShaderStage.input(&_constantBuffer);
        _fragmentShaderStage.input(&_fragmentShaderProgram);
        _fragmentShaderStage.input(&_quadFragment);
        _fragmentShaderStage.output(&_quadFragment);

        // Set OM I/O.
        _outputMergerStage.input(&_depthState);
        _outputMergerStage.input(&_depthRange);
        _outputMergerStage.output(&_renderTarget);


        // kick IA.

        _inputAssemblyStage.prepareReadPrimitive();
        _rasterizeStage.prepareRasterize();

        InputAssemblyStage::Primitive primitive;
        while (_inputAssemblyStage.readPrimitive(&primitive))
        {
            VertexDataB shadedVertices[3];

            for (int i = 0; i < primitive.vertexNum; i++)
            {
                uint16_t vertexIndex = primitive.vertexIndices[i];

                VertexDataA attributeVertex;
                _inputAssemblyStage.readAttributeVertex(vertexIndex, &attributeVertex);

                _vertexShaderStage.executeShader(&attributeVertex, &(shadedVertices[i]));
            }

            outputPrimitive(primitive.primitiveType, shadedVertices, primitive.vertexNum);
        }
    }

    void RenderingContext::outputPrimitive(PrimitiveType primitiveType, const VertexDataB* vertices, int vertexNum)
    {
        // プリミティブをクリップ
        VertexDataB clippedVertices[kClippingPointMaxNum];
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

    void RenderingContext::outputFragment()
    {
        _fragmentShaderStage.execute();

        // TODO:
        // クアッド分出力されたらフラグメントシェーダーステージへ
        // 導関数を使いMIN/MGA判定

        Vector4 c00 = _quadFragment.getQ00()->color;
        Vector4 c01 = _quadFragment.getQ01()->color;
        Vector4 c10 = _quadFragment.getQ10()->color;
        Vector4 c11 = _quadFragment.getQ11()->color;

        if (_quadFragment.getQ00()->isOnPrimitive)
        {
            _outputMergerStage.execute(_quadFragment.getQ00()->x, _quadFragment.getQ00()->y, c00, _quadFragment.getQ00()->depth);
        }
        if (_quadFragment.getQ01()->isOnPrimitive)
        {
            _outputMergerStage.execute(_quadFragment.getQ01()->x, _quadFragment.getQ01()->y, c01, _quadFragment.getQ01()->depth);
        }
        if (_quadFragment.getQ10()->isOnPrimitive)
        {
            _outputMergerStage.execute(_quadFragment.getQ10()->x, _quadFragment.getQ10()->y, c10, _quadFragment.getQ10()->depth);
        }
        if (_quadFragment.getQ11()->isOnPrimitive)
        {
            _outputMergerStage.execute(_quadFragment.getQ11()->x, _quadFragment.getQ11()->y, c11, _quadFragment.getQ11()->depth);
        }
    }

}
