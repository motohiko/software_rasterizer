#pragma once

#include "FrameBuffer.h"
#include "Pipeline\InputAssemblyStageState.h"
#include "Pipeline\VertexShaderStageState.h"
#include "Pipeline\RasterizeStageState.h"
#include "Pipeline\FragmentShaderStageState.h"
#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\ClipStage.h"
#include "Pipeline\PrimitiveAssembly.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Types.h"
#include "Lib\Vector.h"
#include <cstdint>
#include <functional>

namespace SoftwareRasterizer
{
    class RenderingContext : public IFragmentOutput
    {

    private:

        FrameBuffer _frameBuffer;
        InputAssemblyStageState _inputAssemblyStageState;
        VertexShaderStageState _vertexShaderStageState;
        RasterizeStageState _rasterizeStageState;
        FragmentShaderStageState _fragmentShaderStageState;

    public:

        RenderingContext();

        void setFrameSize(int width, int height);
        int getFrameWidth() const;
        int getFrameHeight() const;

        void setFrameColorBuffer(void* addr, size_t widthBytes);
        void setFrameDepthBuffer(void* addr, size_t widthBytes);

        void setClearColor(float r, float g, float b, float a);
        void setClearDepth(float depth);
        void clearFrameBuffer();

        void enableVertexAttribute(int index);
        void disableVertexAttribute(int index);

        void setVertexBuffer(int index, const void* buffer);
        void setVertexAttribute(int index, Semantics semantics, int size, ComponentType type, size_t stride);

        void setIndexBuffer(const uint16_t* indices, int indexNum);

        void setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType);

        void setUniformBlock(const void* uniformBlock);

        void setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain);

        void setViewport(int x, int y, int width, int height);// glViewport
        int getViewportWidth() const;
        int getViewportHeight() const;

        void setDepthRange(float nearVal, float farVal);

        void setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain);

        void drawIndexed();

    private:

        void outputFragment(const Fragment* fragment) override;

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
