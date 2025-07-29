#pragma once

#include "Common.h"
#include "FrameBuffer.h"
#include "InputAssemblyStage.h"
#include "VertexShaderStage.h"
#include "RasterizeStage.h"
#include "FragmentShaderStage.h"
#include "Vector.h"
#include <cstdint>

#include <functional>

namespace SoftwareRenderer
{
    class RenderingContext
    {

    private:

        FrameBuffer _frameBuffer;
        InputAssemblyStage _inputAssemblyStage;
        VertexShaderStage _vertexShaderStage;
        RasterizeStage _rasterizeStage;
        FragmentShaderStage _fragmentShaderStage;

    public:

        void setFrameSize(int width, int height)
        {
            _frameBuffer.setFrameSize(width, height);
        }

        void setFrameBufferRenderColorBuffer(void* addr, size_t widthBytes)
        {
            _frameBuffer.setRenderColorBuffer(addr, widthBytes);
        }

        void setFrameBufferRenderDepthBuffer(void* addr, size_t widthBytes)
        {
            _frameBuffer.setRenderDepthBuffer(addr, widthBytes);
        }

        void enableVertexAttribute(int index)
        {
            _inputAssemblyStage.enableVertexAttribute(index);
        }

        void disableVertexAttribute(int index)
        {
            _inputAssemblyStage.disableVertexAttribute(index);
        }

        void setVertexBuffer(int index, const void* buffer)
        {
            _inputAssemblyStage.setVertexBuffer(index, buffer);
        }

        void setVertexAttribute(int index, Semantics semantics, int size, ComponentType type, size_t stride)
        {
            _inputAssemblyStage.setVertexAttribute(index, semantics, size, type, stride);
        }

        void setIndexBuffer(const uint16_t* indices, int indexNum)
        {
            _inputAssemblyStage.setIndexBuffer(indices, indexNum);
        }

        void setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType)
        {
            _inputAssemblyStage.setPrimitiveTopologyType(primitiveTopologyType);
        }

        void setUniformBlock(const void* uniformBlock)
        {
            _vertexShaderStage.setUniformBlock(uniformBlock);
            _fragmentShaderStage.setUniformBlock(uniformBlock);
        }

        void setVertexShaderProgram(VertexShaderMainFunc vertexShaderMainFunc)
        {
            _vertexShaderStage.setVertexShaderProgram(vertexShaderMainFunc);
        }

        void setViewport(int x, int y, int width, int height)
        {
            _rasterizeStage.setViewport(x, y, width, height);
        }

        int getViewportWidth() const { return _rasterizeStage.getViewportWidth(); }
        int getViewportHeight() const { return _rasterizeStage.getViewportHeight(); }

        void setVaryingVariableCount(int count)
        {
            _rasterizeStage.setVaryingVariableCount(count);
        }

        void setFragmentShaderProgram(FragmentShaderMainFunc fragmentShaderMainFunc)
        {
            _fragmentShaderStage.setFragmentShaderProgram(fragmentShaderMainFunc);
        }

        void clearFrameBuffer()
        {
            _frameBuffer.clearRenderBuffer();
        }

        void drawIndexed();

    private:

        void rasterizeLine(const RasterVertex* p0, const RasterVertex* p1);
        void rasterizeTriangle(const RasterVertex* rasterizationPoint0, const RasterVertex* rasterizationPoint1, const RasterVertex* rasterizationPopint2);

        bool depthTest(int x, int y, float depth)
        {
            float bufferDepth = _frameBuffer.readDepth(x, y);
            bool passed = (depth < bufferDepth);// GL_LESS (OpenGL Default)
            return passed;
        }

    public:

        using OutputPixelHandler = std::function<void()>;
        OutputPixelHandler onOutputPixel;
        void setOnOutputPixel(OutputPixelHandler handler) { onOutputPixel = std::move(handler); }

  };
}
