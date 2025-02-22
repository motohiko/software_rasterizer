#pragma once

#include <cstdint>
#include <cfloat>
#include <cassert>
#include <algorithm>

#include "Algorithm.h"
#include "Vector.h"
#include "Matrix.h"
#include "VertexProcessedFlags.h" 

#define TEST_PERSPECTIVE_CORRECT_VARYING_VARIABLES

namespace MyApp
{
    const int kVertexElementsNum = 16;
    const int kVaryingVariablesNum = 16;

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
        Vector4 elements[kVertexElementsNum];
    };

    struct VertexShaderOutput
    {
        Vector4 position;// =gl_Position, clip space coordinates
        Vector4 varyingVariables[kVaryingVariablesNum];
    };

    struct PixelShaderInput
    {
        const void* uniformBlock;
        Vector4 fragcoord;// =gl_fragcoord (xy = screen space coordinates, z = depth, w = 1 / clip.w )
        const Vector4* varyingVariables;
    };

    struct PixelShaderOutput
    {
        Vector4 fragColor;// =gl_FragColor 
    };

    typedef void (*VertexShaderMainFunc)(const VertexShaderInput* input, VertexShaderOutput* output);
    typedef void (*PixelShaderMainFunc)(const PixelShaderInput* input, PixelShaderOutput* output);

    struct InterpPoint
    {
        Vector4 position;
        Vector4 varyingVariables[kVaryingVariablesNum];
    };

    typedef InterpPoint ClippingPoint;
    typedef InterpPoint ScreenPoint;

//    struct ClippingPoint
//    {
//        Vector4 position;
//        Vector4 varyingVariables[kVaryingVariablesNum];
//    };

//    struct ScreenPoint
//    {
//        Vector4 position;
//        Vector4 varyingVariables[kVaryingVariablesNum];
//    };

    class Renderer
    {

    private:
        int _viewportOffsetX = 0;
        int _viewportOffsetY = 0;
        int _viewportWidth = 0;
        int _viewportHeight = 0;
        ColorFrameBuffer* _colorFrameBuffer = nullptr;
        DepthFrameBuffer* _depthFrameBuffer = nullptr;
        const void* _uniformBlock = nullptr;
        PrimitiveTopologyType _primitiveTopologyType = PrimitiveTopologyType::LineList;
        VertexBufferElement _vertexElements[kVertexElementsNum] = {};
        int _vertexBufferElementsCount = 0;;
        const uint16_t* _indexBufferIndices = nullptr;
        int _indexBufferIndicesNum = 0;
        VertexShaderMainFunc _vertexShaderMainFunc = nullptr;
        PixelShaderMainFunc _pixelShaderMainFunc = nullptr;
        int _varyingVariablesCount = 0;

    public:

        void setViewport(int x, int y, int width, int height)
        {
            _viewportOffsetX = x;
            _viewportOffsetY = y;
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
            _vertexBufferElementsCount = count;
        }

        void setIndexBuffer(const uint16_t* indices, int indicesNum)
        {
            _indexBufferIndices = indices;
            _indexBufferIndicesNum = indicesNum;
        }

        void setVertexShaderProgram(VertexShaderMainFunc vertexShaderMainFunc)
        {
            _vertexShaderMainFunc = vertexShaderMainFunc;
        }

        void setPixelShaderProgram(PixelShaderMainFunc pixelShaderMainFunc)
        {
            _pixelShaderMainFunc = pixelShaderMainFunc;
        }

        void setVaryingVariablesCount(int count)
        {
            assert(0 <= _varyingVariablesCount && _varyingVariablesCount < kVaryingVariablesNum);
            _varyingVariablesCount = count;
        }

        void clearFrameBuffer();

    private:

        void InputAssembly(int index, Vector4* elements);

        void clipPrimitiveLine(const VertexShaderOutput* primitiveVertices, ClippingPoint* clippedPoints, int* clippingPointsCount);
        void clipPrimitiveTriangle(const VertexShaderOutput* primitiveVertices, ClippingPoint* clippingPoints, int* clippingPointsCount);

        void rasterizeLinePixel(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1, int x, int y);
        void rasterizeLine(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1);
        void rasterizeTriangle(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1, const ScreenPoint* screenPoint2);

        void outputPrimitiveLine(const VertexShaderOutput* vertexShaderOutputs);
        void outputPrimitiveTriangle(const VertexShaderOutput* vertexShaderOutputs);

    public:

        void drawIndexed();

    private:

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
            return clipSpaceVertex.w - (clippingPlaneParameter->sign * clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex));
    #endif
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

        void setPixel(int x, int y, const Vector4& color, float depth)
        {
            int depthOffset = (_depthFrameBuffer->widthBytes * y) + (sizeof(float) * x);
            assert(0 <= depthOffset && depthOffset < (_depthFrameBuffer->widthBytes * _depthFrameBuffer->height));
            if (depthOffset < 0 || (_depthFrameBuffer->widthBytes * _depthFrameBuffer->height) <= depthOffset)
            {
                return;
            }
            float* depthDst = (float*)(((uintptr_t)(_depthFrameBuffer->addr)) + (_depthFrameBuffer->widthBytes * y) + (sizeof(float) * x));
            *depthDst = depth;

            int colorOffset = (_colorFrameBuffer->widthBytes * y) + (sizeof(uint32_t) * x);
            assert(0 <= colorOffset && colorOffset < (_colorFrameBuffer->widthBytes * _colorFrameBuffer->height));
            if (colorOffset < 0 || (_colorFrameBuffer->widthBytes * _colorFrameBuffer->height) <= colorOffset)
            {
                return;
            }
            uint32_t* colorDst = (uint32_t*)(((uintptr_t)(_colorFrameBuffer->addr)) + colorOffset);
            uint32_t r = (uint8_t)(255 * clamp(color.x, 0.0f, 1.0f));
            uint32_t g = (uint8_t)(255 * clamp(color.y, 0.0f, 1.0f));
            uint32_t b = (uint8_t)(255 * clamp(color.z, 0.0f, 1.0f));
            *colorDst = (r << 16) | (g << 8) | (b);
        }

        static float remapDepth(float value, float in_min, float in_max, float out_min, float out_max)
        {
            return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }




        //
        // Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization 
        // 
        static float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

        struct BoundingBox
        {
            float minX;
            float minY;
            float maxX;
            float maxY;

            void Init()
            {
                minX = FLT_MAX;
                minY = FLT_MAX;
                maxX = -FLT_MAX;
                maxY = -FLT_MAX;
            }

            void addPoint(const Vector4& p)
            {
                minX = std::min(minX, p.x);
                minY = std::min(minY, p.y);
                maxX = std::max(maxX, p.x);
                maxY = std::max(maxY, p.y);
            }
        };



        void lerpClippingPoint(ClippingPoint* dst, const ClippingPoint& p0, const ClippingPoint& p1, float t)
        {
            dst->position = Vector4::Lerp(p0.position, p1.position, t);
            for (int i = 0; i < _varyingVariablesCount; ++i)
            {
                dst->varyingVariables[i] = Vector4::Lerp(p0.varyingVariables[i], p1.varyingVariables[i], t);
            }
        }



        // 
        void transformScreenCoordinates(Renderer& renderingContext, const ClippingPoint* clippingPoint, ScreenPoint* screenPoint)
        {
#ifndef NDEBUG
            //float lazyW = std::abs(clippingPoint->position.w) + 0.00001f;
            //assert(-lazyW <= clippingPoint->position.x && clippingPoint->position.x <= lazyW);
            //assert(-lazyW <= clippingPoint->position.y && clippingPoint->position.y <= lazyW);
            //assert(-lazyW <= clippingPoint->position.z && clippingPoint->position.z <= lazyW);
#endif

            float w = clippingPoint->position.w;
            assert(w != 0.0f);

            // 正規化デバイス座標へ変換（w除算）
            Vector4 ndcPosition = clippingPoint->position / w;

            // ビューポート変換
            screenPoint->position.x = (ndcPosition.x + 1.0f) * ((float)(_viewportWidth) / 2.0f);
            screenPoint->position.y = (ndcPosition.y + 1.0f) * ((float)(_viewportHeight) / 2.0f);
            //assert(screenPoint->position.z == ndcPosition.z);
            //assert(screenPoint->position.w == 1.0f);

            //int x = screenPoint->position.x;
            //int y = screenPoint->position.y;
            //int width = _colorFrameBuffer->width;
            //int height = _colorFrameBuffer->height;
            //assert(0 <= x && x < width);
            //assert(0 <= y && y < height);

            screenPoint->position.z = ndcPosition.z;
            screenPoint->position.w = 1.0f;
#ifdef TEST_PERSPECTIVE_CORRECT_VARYING_VARIABLES
            screenPoint->position.w /= w;
#endif

            // 補間値はW除算（パースペクティブコレクト）
            for (int i = 0; i < _varyingVariablesCount; ++i)
            {
                screenPoint->varyingVariables[i] = clippingPoint->varyingVariables[i];
#ifdef TEST_PERSPECTIVE_CORRECT_VARYING_VARIABLES
                screenPoint->varyingVariables[i] /= w;
#endif
            }
        }

        static bool faceCulling(const Vector2* ndcPositions)
        {
            // glEnable(GL_CULL_FACE)
            // glFrontFace(GL_CCW)

            const Vector2& a = ndcPositions[0];
            const Vector2& b = ndcPositions[1];
            const Vector2& c = ndcPositions[2];
            float n = (b - a).cross(c - a);

            if (false)// CW
            {
                // 
                //      * b
                //     /
                //    /
                // a *----* c
                //   |
                //   | n
                //
                // n = ab x ac
                // n < 0 : front face
                //

                if (0.0f <= n)// back face
                {
                    return false;
                }

            }
            else// CCW (OpenGL default)
            {
                // 
                //      * c
                // n | /
                //   |/
                // a *----* b
                //
                // n = ab x ac
                // 0 < n : front face
                //

                if (n <= 0.0f)// back face
                {
                    return false;
                }
            }

            return true;
        }

    };

}
