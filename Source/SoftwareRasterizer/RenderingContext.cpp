#include "RenderingContext.h"
#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Modules\ClipStage.h"
#include "Modules\PrimitiveAssembly.h"
#include "Modules\TextureOperations.h" 
#include <iterator>// std::size
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
        // OM
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
        // OM
        //assert(nullptr != addr);
        //assert(0 <= width);
        //assert(0 <= height);
        //assert(width <= widthBytes);
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
        TextureOperations::FillTextureColor(&(_renderTarget.colorBuffer), _clearColor);
        TextureOperations::FillTextureDepth(&(_renderTarget.depthBuffer), _clearDepth);
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

    void RenderingContext::setVertexAttribute(int index, int size, ComponentType type, size_t stride, const void* buffer)
    {
        // IA
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
        // RS
        _depthRange.depthRangeNearVal = nearVal;
        _depthRange.depthRangeFarVal = farVal;
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
        _rasterizeStage.input(&_windowSize);
        _rasterizeStage.input(&_rasterizerState);
        _rasterizeStage.input(&_viewport);
        _rasterizeStage.input(&_depthRange);
        _rasterizeStage.ouput(this);

        // PS I/O
        _fragmentShaderStage.input(&_constantBuffer);
        _fragmentShaderStage.input(&_fragmentShaderProgram);

        // OM I/O
        _outputMergerStage.input(&_depthState);
        _outputMergerStage.input(&_depthRange);
        _outputMergerStage.ouput(&_renderTarget);


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

        // クアッド分出力されたらフラグメントシェーダーステージへ

        // 導関数を使いMIN/MGA判定

    }

}
