#pragma once

#include <cstdint>
#include <cfloat>
#include <cassert>
#include <algorithm>

#include "Algorithm.h"
#include "Vector.h"
#include "Matrix.h"

namespace MyApp
{
    const int kVertexElementNum = 16;
    const int kVaryingVariableNum = 16;

    const int kClippingPlaneNum = 6;

    struct ClippingPlaneParameter
    {
        int vectorComponentIndex;// 0=x, 1=y, 2=z
        float sign;
    };

    const ClippingPlaneParameter kClippingPlaneParameters[kClippingPlaneNum] =
    {
        { 0,  1.0f },
        { 0, -1.0f },
        { 1,  1.0f },
        { 1, -1.0f },
        { 2,  1.0f },
        { 2, -1.0f }
    };

    const int kTriangleClippingPointMaxNum = 7;

    struct ColorFrameBuffer
    {
        void* addr;// BGRA
        int width;
        int height;
        int widthBytes;
    };

    struct DepthFrameBuffer
    {
        void* addr;// float
        int width;
        int height;
        int widthBytes;
    };

    struct VertexBufferElement
    {
        const void* buffer;
        size_t strideInBytes;
    };

    enum PrimitiveTopologyType
    {
        LineList,
        TriangleList,
    };

    struct VertexShaderInput
    {
        const void* uniformBlock;
        Vector4 elements[kVertexElementNum];
    };

    struct VertexShaderOutput
    {
        Vector4 position;// =gl_Position, clip space coordinates
        Vector4 varyingVariables[kVaryingVariableNum];
    };

    struct PixelShaderInput
    {
        const void* uniformBlock;
        Vector4 fragcoord;// =gl_fragcoord (xy = screen space coordinates, z = depth, w = 1.0 )
        const Vector4* varyingVariables;
    };

    struct PixelShaderOutput
    {
        Vector4 fragColor;// =gl_FragColor 
    };

    typedef void (*VertexShaderMainFunc)(const VertexShaderInput* input, VertexShaderOutput* output);
    typedef void (*PixelShaderMainFunc)(const PixelShaderInput* input, PixelShaderOutput* output);

    class Renderer
    {

    private:

        int _viewportX = 0;
        int _viewportY = 0;
        int _viewportWidth = 0;
        int _viewportHeight = 0;

        ColorFrameBuffer* _colorFrameBuffer = nullptr;
        DepthFrameBuffer* _depthFrameBuffer = nullptr;

        const void* _uniformBlock = nullptr;

        PrimitiveTopologyType _primitiveTopologyType = PrimitiveTopologyType::LineList;
        VertexBufferElement _vertexElements[kVertexElementNum] = {};
        int _vertexBufferElementCount = 0;;
        const uint16_t* _indexBufferIndices = nullptr;
        int _indexBufferIndexCount = 0;
        int _varyingVariableCount = 0;

        VertexShaderMainFunc _vertexShaderMainFunc = nullptr;
        PixelShaderMainFunc _pixelShaderMainFunc = nullptr;

    public:

        void setViewport(int x, int y, int width, int height)
        {
            _viewportX = x;
            _viewportY = y;
            _viewportWidth = width;
            _viewportHeight = height;
        }

        int getViewportWidth() const { return _viewportWidth; }
        int getViewportHeight() const { return _viewportHeight; }

        void setFrameBuffer(ColorFrameBuffer* colorFrameBuffer, DepthFrameBuffer* depthFrameBuffer)
        {
            _colorFrameBuffer = colorFrameBuffer;
            _depthFrameBuffer = depthFrameBuffer;
        }

        void setUniformBlock(const void* uniformBlock)
        {
            _uniformBlock = uniformBlock;
        }

        void setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType)
        {
            _primitiveTopologyType = primitiveTopologyType;
        }

        void setVertexBufferElement(int index, const void* buffer, size_t strideInBytes)
        {
            assert(0 <= index && index < std::size(_vertexElements));
            _vertexElements[index].buffer = buffer;
            _vertexElements[index].strideInBytes = strideInBytes;
        }

        void setVertexBufferElementsCount(int count)
        {
            assert(0 <= count && count < std::size(_vertexElements));
            _vertexBufferElementCount = count;
        }

        void setIndexBuffer(const uint16_t* indices, int indicesNum)
        {
            _indexBufferIndices = indices;
            _indexBufferIndexCount = indicesNum;
        }

        void setVertexShaderProgram(VertexShaderMainFunc vertexShaderMainFunc)
        {
            _vertexShaderMainFunc = vertexShaderMainFunc;
        }

        void setPixelShaderProgram(PixelShaderMainFunc pixelShaderMainFunc)
        {
            _pixelShaderMainFunc = pixelShaderMainFunc;
        }

        void setVaryingVariableCount(int count)
        {
            assert(0 <= _varyingVariableCount && _varyingVariableCount < kVaryingVariableNum);
            _varyingVariableCount = count;
        }

        void clearFrameBuffer();

        void drawIndexed();

    private:

        void inputAssembly(int index, Vector4* elements);

    private:

        void outputPrimitiveLine(const VertexShaderOutput* vertexShaderOutputs, int vertexShaderOutputsCount);
        void outputPrimitiveTriangle(const VertexShaderOutput* vertexShaderOutputs, int vertexShaderOutputCount);

    private:

        struct ClippedPrimitiveVertex
        {
            Vector4 position;// clip space coordinates
            Vector4 varyingVariables[kVaryingVariableNum];
        };

        void clipPrimitiveLine(const VertexShaderOutput* primitiveVertices, int primitiveVertexCount, ClippedPrimitiveVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount);
        void clipPrimitiveTriangle(const VertexShaderOutput* primitiveVertices, int primitiveVertexCount, ClippedPrimitiveVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount);

        // クリップ境界座標系へ変換
        // ret : outside <= 0 < indide
        //
        // Blinn & Newell（1978）Clipping Using Homogeneous Coordinates - section 2. CLIPPING
        // SIGGRAPH '78: Proceedings of the 5th annual conference on Computer graphics and interactive techniques Pages 245 - 251
        //
        static float transformClippingBoundaryCoordinate(const Vector4& clipSpaceVertex, const ClippingPlaneParameter* clippingPlaneParameter)
        {
#if !0
            float clippingPoint = clipSpaceVertex.w * clippingPlaneParameter->sign;
            float offsetFromClippingPoint = clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex) - clippingPoint;
            // sign = 1.0f なら符号を反転
            return (clippingPlaneParameter->sign * -1.0f) * offsetFromClippingPoint;
#else
            // 最適化
            return clipSpaceVertex.w - (clippingPlaneParameter->sign * clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex));
#endif
        }

        void lerpPrimitiveVertex(ClippedPrimitiveVertex* dst, const ClippedPrimitiveVertex& p0, const ClippedPrimitiveVertex& p1, float t)
        {
            // ※（クリップ空間座標系の）w はソースの z（を符号反転したもの）が入っているので、xyz と一緒に線形補間する

            dst->position = Vector4::Lerp(p0.position, p1.position, t);
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                dst->varyingVariables[i] = Vector4::Lerp(p0.varyingVariables[i], p1.varyingVariables[i], t);
            }
        }


    private:

        struct RasterizationPopint
        {
            Vector4 ndcPosition;// normalized device coordinates
            Vector2 windowPosition;// window coordinates
            float depth;
            float perspectiveCorrectInvW;
            Vector4 varyingVariablesDividedByW[kVaryingVariableNum];// divided by w
        };

        void makeRasterizationPopint(const ClippedPrimitiveVertex* clippedPrimitiveVertices, RasterizationPopint* rasterizationPopint);

        void rasterizeLine(const RasterizationPopint* rasterizationPopint0, const RasterizationPopint* rasterizationPopint1);
        void rasterizeLinePixel(const RasterizationPopint* p0, const RasterizationPopint* p1, int x, int y);

        void rasterizeTriangleFaceOnly(const RasterizationPopint* rasterizationPopint0, const RasterizationPopint* rasterizationPopint1, const RasterizationPopint* rasterizationPopint2);
        void rasterizeTrianglePixel(const RasterizationPopint* p0, const RasterizationPopint* p1, const RasterizationPopint* p2, float a, int x, int y);

        void outputPixel(int x, int y, const Vector4& color, float depth);

        static float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

        bool depthTest(int x, int y, float depth)
        {
            int depthOffset = (_depthFrameBuffer->widthBytes * y) + (sizeof(float) * x);
            assert(0 <= depthOffset && depthOffset < (_depthFrameBuffer->widthBytes * _depthFrameBuffer->height));
            if (depthOffset < 0 || (_depthFrameBuffer->widthBytes * _depthFrameBuffer->height) <= depthOffset)
            {
                return false;
            }

            float* depthDst = (float*)(((uintptr_t)(_depthFrameBuffer->addr)) + depthOffset);
            bool pass = (depth < *depthDst);// GL_LESS (OpenGL Default)
            return pass;
        }

    };
}
