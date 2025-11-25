#pragma once

#include "Pipeline\InputAssemblyStage.h"
#include "Pipeline\VertexShaderStage.h"
#include "Pipeline\RasterizeStage.h"
#include "Pipeline\FragmentShaderStage.h"
#include "Pipeline\OutputMergerStage.h"
#include "State\WindowSize.h"
#include "State\RenderTarget.h"
#include "State\ClearParam.h"
#include "State\InputLayout.h"
#include "State\VertexBuffers.h"
#include "State\IndexBuffer.h"
#include "State\ConstantBuffer.h"
#include "State\VertexShaderProgram.h"
#include "State\RasterizerState.h"
#include "State\Viewport.h"
#include "State\DepthRange.h"
#include "State\FragmentShaderProgram.h"
#include "State\DepthState.h"
#include "Core\Types.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    class RenderingContext
    {

    public:

        RenderingContext();

        void setWindowSize(int width, int height);
        int getWindowWidth() const;
        int getWindowHeight() const;

        void setRenderTargetColorBuffer(void* addr, int width, int height, int widthBytes);
        void setRenderTargetDepthBuffer(void* addr, int width, int height, int widthBytes);

        void setClearColor(float red, float green, float blue, float alpha);// glClearColor
        void setClearDepth(float depth);// glClearDepth
        void clearRenderTarget();// glClear

        void enableVertexAttribute(int index);// glEnableVertexAttribArray
        void disableVertexAttribute(int index);// glDisableVertexAttribArray
        void setVertexAttribute(int index, int size, ComponentDataType type, size_t stride, const void* buffer);// glVertexAttribPointer

        void setIndexBuffer(const uint16_t* indices, int indexNum);// glBufferData

        void setUniformBlock(const void* uniformBlock);

        void setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain);// glUseProgram

        void setViewport(int x, int y, int width, int height);// glViewport
        int getViewportWidth() const;
        int getViewportHeight() const;

        void setDepthRange(float nearVal, float farVal);// glDepthRange

        void setFrontFaceMode(FrontFaceMode frontFaceMode);// glFrontFace
        void setCullFaceMode(CullFaceMode cullFaceMode);// glCullFace

        void setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain);// glUseProgram

        void setDepthFunc(ComparisonFuncType depthFunc);// glDepthFunc

        void drawIndexed(PrimitiveTopologyType primitiveTopologyType);

    private:

        void outputPrimitive(PrimitiveType primitiveType, const VertexDataB* vertices, int vertexNum);
        void outputFragment();

    private:

        WindowSize _windowSize;
        ClearParam _clearParam;
        ConstantBuffer _constantBuffer;                 // VS / PS
        InputLayout _inputLayout;                       // IA
        VertexBuffers _vertexBuffers;                   // IA
        IndexBuffer _indexBuffer;                       // IA
        VertexShaderProgram _vertexShaderProgram;       // VS
        RasterizerState _rasterizerState;               // RS
        Viewport _viewport;                             // RS
        DepthRange _depthRange;                         // RS
        FragmentShaderProgram _fragmentShaderProgram;   // PS
        RenderTarget _renderTarget;                     // OM
        DepthState _depthState;                         // OM

    private:

        // Pipeline
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

        // I/O data.
        FragmentDataA _q00 = {};
        FragmentDataA _q01 = {};
        FragmentDataA _q10 = {};
        FragmentDataA _q11 = {};
        QuadFragmentDataA _quadFragment = {};

    };
}
