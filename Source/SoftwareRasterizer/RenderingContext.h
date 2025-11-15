#pragma once

#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Pipeline\InputAssemblyStageState.h"
#include "Pipeline\VertexShaderStageState.h"
#include "Pipeline\RasterizeStageState.h"
#include "Pipeline\FragmentShaderStageState.h"
#include "FrameBuffer.h"
#include "Types.h"
#include <cstdint>
#include <memory>

namespace SoftwareRasterizer
{
    class RenderingContext
    {

    public:

        RenderingContext();

        void setFrameSize(int width, int height);
        int getFrameWidth() const;
        int getFrameHeight() const;

        void setFrameColorBuffer(void* addr, size_t widthBytes);
        void setFrameDepthBuffer(void* addr, size_t widthBytes);

        void setClearColor(float r, float g, float b, float a);// glClearColor
        void setClearDepth(float depth);// glClearDepth
        void clearFrameBuffer();// glClear

        void enableVertexAttribute(int index);
        void disableVertexAttribute(int index);
        void setVertexBuffer(int index, const void* buffer);
        void setVertexAttribute(int index, SemanticsType semantics, int size, ComponentType type, size_t stride);

        void setIndexBuffer(const uint16_t* indices, int indexNum);

        void setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType);

        void setUniformBlock(const void* uniformBlock);

        void setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain);

        void setViewport(int x, int y, int width, int height);// glViewport
        int getViewportWidth() const;
        int getViewportHeight() const;

        void setDepthRange(float nearVal, float farVal);// glDepthRange

        void setFrontFaceType(FrontFaceType frontFacetype);// glFrontFace
        void setCullFaceType(CullFaceType cullFaceType);// glCullFace

        void setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain);

        void setDepthFunc(ComparisonType depthFunc);// glDepthFunc

        void drawIndexed();

    private:

        void outputPrimitive(PrimitiveType primitiveType, const ShadedVertex* vertices, int vertexNum);
        void outputFragment(const Fragment* fragment);

        bool depthTest(int x, int y, float depth);

    private:

        FrameBuffer _frameBuffer;

        InputAssemblyStageState _inputAssemblyStageState;
        VertexShaderStageState _vertexShaderStageState;
        RasterizeStageState _rasterizeStageState;
        FragmentShaderStageState _fragmentShaderStageState;
        ComparisonType _depthFunc = ComparisonType::kDefault;

        InputAssemblyStage _inputAssemblyStage;
        VertexShaderStage _vertexShaderStage;
        RasterizeStage _rasterizeStage;
        FragmentShaderStage _fragmentShaderStage;

        friend class InputAssemblyStage;
        friend class VertexShaderStage;
        friend class RasterizeStage;
        friend class FragmentShaderStage;
    };
}
