#pragma once

#include "Common.h"
#include "Vector.h"

namespace SoftwareRenderer
{
    const int kTriangleClippingPointMaxNum = 7;

    const int kClippingPlaneNum = 6;

    struct ClippingPlaneParameter
    {
        int vectorComponentIndex;// 0=x, 1=y, 2=z
        float sign;
    };

    const ClippingPlaneParameter kClipPlaneParameters[kClippingPlaneNum] =
    {
        { 0, -1.0f },// left
        { 0,  1.0f },// right
        { 1, -1.0f },// bottom
        { 1,  1.0f },// top
        { 2, -1.0f },// near
        { 2,  1.0f } // far
    };

    struct NdcVertex
    {
        Vector3 ndcPosition;// normalized device coordinates
    };

    struct RasterVertex
    {
        Vector2 wrcPosition;// window relative coordinate
        float depth;
        float invW;
        Vector4 varyingVariablesDividedByW[kMaxVaryingVectors];
    };

    class RasterizeStage
	{

    private:

        int _varyingVariableCount = 0;

        int _viewportX = 0;
        int _viewportY = 0;
        int _viewportWidth = 0;
        int _viewportHeight = 0;

    public:

        void setVaryingVariableCount(int count);

        void setViewport(int x, int y, int width, int height);
        int getViewportWidth() const { return _viewportWidth; }
        int getViewportHeight() const { return _viewportHeight; }

        void clipPrimitiveLine(const Vertex* primitiveVertices, int primitiveVertexCount, Vertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;
        void clipPrimitiveTriangle(const Vertex* primitiveVertices, int primitiveVertexCount, Vertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const;

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
            // 最適化
            return clipSpaceVertex.w - (clippingPlaneParameter->sign * clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex));
#endif
        }

        void lerpVertex(Vertex* dst, const Vertex& p0, const Vertex& p1, float t) const
        {
            // ※ w も xyz と同様に線形補間する

            dst->clipSpacePosition = Vector4::Lerp(p0.clipSpacePosition, p1.clipSpacePosition, t);
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                dst->varyingVariables[i] = Vector4::Lerp(p0.varyingVariables[i], p1.varyingVariables[i], t);
            }
        }

    public:

        void transformToNdcVertex(const Vertex* vertex, NdcVertex* ndcVertex) const;
        void transformRasterVertex(const Vertex* clippedPrimitiveVertices, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const;

        void getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment);
        void getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment);

    public:

        // 面上のある点が三角形の内部にあるかどうかを判定
         //
         // Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization.
         //
        static float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

    };
}
