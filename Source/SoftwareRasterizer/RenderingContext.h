#pragma once

#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Pipeline\OutputMergerStage.h"
#include "State\RenderTarget.h"
#include "State\InputLayout.h"
#include "State\VertexBuffers.h"
#include "State\IndexBuffer.h"
#include "State\ConstantBuffer.h"
#include "State\VertexShaderProgram.h"
#include "State\RasterizerState.h"
#include "State\Viewport.h"
#include "State\FragmentShaderProgram.h"
#include "State\DepthState.h"
#include "Types.h"
#include <cstdint>
#include <memory>

namespace SoftwareRasterizer
{
    class RenderingContext
    {

    public:

        RenderingContext();

        void setWindowSize(int width, int height);
        int getWindowWidth() const;
        int getWindowHeight() const;

        void setRenderTargetColorBuffer(void* addr, int width, int height, size_t widthBytes);
        void setRenderTargetDepthBuffer(void* addr, int width, int height, size_t widthBytes);

        void setClearColor(float r, float g, float b, float a);// glClearColor
        void setClearDepth(float depth);// glClearDepth
        void clearRenderTarget();// glClear

        void enableVertexAttribute(int index);// glEnableVertexAttribArray
        void disableVertexAttribute(int index);// glDisableVertexAttribArray
        void setVertexAttribute(int index, SemanticsType semantics, int size, ComponentType type, size_t stride, const void* buffer);// glVertexAttribPointer

        void setIndexBuffer(const uint16_t* indices, int indexNum);// glBufferData

        void setUniformBlock(const void* uniformBlock);

        void setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain);// glUseProgram

        void setViewport(int x, int y, int width, int height);// glViewport
        int getViewportWidth() const;
        int getViewportHeight() const;

        void setDepthRange(float nearVal, float farVal);// glDepthRange

        void setFrontFaceType(FrontFaceType frontFacetype);// glFrontFace
        void setCullFaceType(CullFaceType cullFaceType);// glCullFace

        void setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain);// glUseProgram

        void setDepthFunc(ComparisonType depthFunc);// glDepthFunc

        void drawIndexed(PrimitiveTopologyType primitiveTopologyType);

    private:

        void clearRenderTargetColorBuffer();
        void clearRenderTargetDepthBuffer();

        void outputPrimitive(PrimitiveType primitiveType, const ShadedVertex* vertices, int vertexNum);
        void outputFragment(const Fragment* fragment);

    private:

        int _windowWidth = 0;
        int _windowHeight = 0;

        Vector4 _clearColor = Vector4::kZero;
        float _clearDepth = 1.0f;

        // VS / PS
        ConstantBuffer _constantBuffer;

        // IA
        InputLayout _inputLayout;
        VertexBuffers _vertexBuffers;
        IndexBuffer _indexBuffer;

        // VS
        VertexShaderProgram _vertexShaderProgram;

        // RS
        RasterizerState _rasterizerState;
        Viewport _viewport;

        // PS
        FragmentShaderProgram _fragmentShaderProgram;

        // OM
        RenderTarget _renderTarget;
        DepthState _depthState;

    private:

        InputAssemblyStage _inputAssemblyStage;     // IA
        VertexShaderStage _vertexShaderStage;       // VS
        RasterizeStage _rasterizeStage;             // RS
        FragmentShaderStage _fragmentShaderStage;   // PS
        OutputMergerStage _outputMergerStage;       // OM

        friend class InputAssemblyStage;
        friend class VertexShaderStage;
        friend class RasterizeStage;
        friend class FragmentShaderStage;
        friend class OutputMergerStage;

    };
}
