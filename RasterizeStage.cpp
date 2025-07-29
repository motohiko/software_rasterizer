#include "RasterizeStage.h"
#include "Algorithm.h"
#include <cassert>
#include <cmath>

namespace SoftwareRenderer
{
    void RasterizeStage::setVaryingVariableCount(int count)
    {
        assert(0 <= _varyingVariableCount && _varyingVariableCount < kMaxVaryingVectors);
        _varyingVariableCount = count;
    }

    void RasterizeStage::setViewport(int x, int y, int width, int height)
    {
        _viewportX = x;
        _viewportY = y;
        _viewportWidth = width;
        _viewportHeight = height;
    }

    void RasterizeStage::clipPrimitiveLine(const Vertex* primitiveVertices, int primitiveVertexCount, Vertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const
    {
        assert(primitiveVertexCount == 2);
        if (primitiveVertexCount != 2)
        {
            return;
        }

        for (int j = 0; j < 2; ++j)
        {
            clippedPrimitiveVertices[j].clipSpacePosition = primitiveVertices[j].clipSpacePosition;
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                clippedPrimitiveVertices[j].varyingVariables[i] = primitiveVertices[j].varyingVariables[i];
            }
        }

        for (int i = 0; i < kClippingPlaneNum; ++i)
        {
            // 境界座標系に変換
            float d0 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[0].clipSpacePosition, &kClipPlaneParameters[i]);
            float d1 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[1].clipSpacePosition, &kClipPlaneParameters[i]);
            if (0.0f < d0)
            {
                if (d1 < 0.0f)
                {
                    // d0: indide, d1: outside
                    const float t = d0 / (d0 - d1);
                    lerpVertex(&clippedPrimitiveVertices[1], clippedPrimitiveVertices[0], clippedPrimitiveVertices[1], t);
                }
                else
                {
                    // d0: indide, d1: inside
                }
            }
            else if (0.0f <= d1)
            {
                // d0: outside, d1: inside
                const float t = d0 / (d0 - d1);
                lerpVertex(&clippedPrimitiveVertices[0], clippedPrimitiveVertices[0], clippedPrimitiveVertices[1], t);
            }
            else
            {
                // d0: outside, d1: outside
                return;
            }
        }

        *clippedPrimitiveVertiexCount = 2;

#ifndef NDEBUG
        float lazyW = std::abs(clippedPrimitiveVertices[0].clipSpacePosition.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertices[0].clipSpacePosition.x && clippedPrimitiveVertices[0].clipSpacePosition.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[0].clipSpacePosition.y && clippedPrimitiveVertices[0].clipSpacePosition.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[0].clipSpacePosition.z && clippedPrimitiveVertices[0].clipSpacePosition.z <= lazyW);

        lazyW = std::abs(clippedPrimitiveVertices[1].clipSpacePosition.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertices[1].clipSpacePosition.x && clippedPrimitiveVertices[1].clipSpacePosition.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[1].clipSpacePosition.y && clippedPrimitiveVertices[1].clipSpacePosition.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[1].clipSpacePosition.z && clippedPrimitiveVertices[1].clipSpacePosition.z <= lazyW);
#endif
    }

    void RasterizeStage::clipPrimitiveTriangle(const Vertex* primitiveVertices, int primitiveVertexCount, Vertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const
    {
        // Sutherland-Hodgman algorithm
        // Ivan Sutherland, Gary W. Hodgman: Reentrant Polygon Clipping. Communications of the ACM, vol. 17, pp. 32-42, 1974
        //
        // see https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
        //
        // Pseudocode
        // 
        //    List outputList = subjectPolygon;
        //
        //    for (Edge clipEdge in clipPolygon) do
        //        List inputList = outputList;
        //        outputList.clear();
        //
        //        for (int i = 0; i < inputList.count; i += 1) do
        //            Point current_point = inputList[i];
        //            Point prev_point = inputList[(i - 1) % inputList.count];
        //
        //            Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
        //
        //            if (current_point inside clipEdge) then
        //                if (prev_point not inside clipEdge) then
        //                    outputList.add(Intersecting_point);
        //                end if
        //                outputList.add(current_point);
        //
        //            else if (prev_point inside clipEdge) then
        //                outputList.add(Intersecting_point);
        //            end if
        //
        //        done
        //    done
        //

        assert(primitiveVertexCount == 3);
        if (primitiveVertexCount != 3)
        {
            return;
        }

        Vertex inputList[kTriangleClippingPointMaxNum] = {};
        int inputListCount = 0;

        Vertex outputList[kTriangleClippingPointMaxNum] = {};
        int outputListCount = 0;

        // List outputList = subjectPolygon;
        for (int j = 0; j < 3; j++)
        {
            outputList[outputListCount].clipSpacePosition = primitiveVertices[j].clipSpacePosition;
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                outputList[outputListCount].varyingVariables[i] = primitiveVertices[j].varyingVariables[i];
            }
            outputListCount++;
        }

        //for (Edge clipEdge in clipPolygon) do
        for (int k = 0; k < kClippingPlaneNum; k++)
        {
            // List inputList = outputList;
            // outputList.clear();
            for (int j = 0; j < outputListCount; ++j)
            {
                inputList[j] = outputList[j];
            }
            inputListCount = outputListCount;
            outputListCount = 0;

            // for (int i = 0; i < inputList.count; i += 1) do
            for (int j = 0; j < inputListCount; j++)
            {
                // Point current_point = inputList[i];
                // Point prev_point = inputList[(i - 1) % inputList.count];
                Vertex& currentPoint = inputList[j];
                Vertex& prevPoint = inputList[((j - 1) + inputListCount) % inputListCount];// (0 - 1) % n = -1 になるので、 n を足してから余剰を求める

                Vertex& p0 = prevPoint;
                Vertex& p1 = currentPoint;

                // 境界座標系に変換（0 <= d のとき indide）
                float d0 = transformClippingBoundaryCoordinate(p0.clipSpacePosition, &kClipPlaneParameters[k]);
                float d1 = transformClippingBoundaryCoordinate(p1.clipSpacePosition, &kClipPlaneParameters[k]);

                Vertex intersectingPoint;

                // current_point inside clipEdge
                if (0.0f <= d1)
                {
                    // prev_point not inside clipEdge
                    if (d0 < 0.0f)
                    {
                        // Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
                        float t = d1 / (d1 - d0);
                        lerpVertex(&intersectingPoint, p1, p0, t);

                        // outputList.add(Intersecting_point);
                        if (!(outputListCount < kTriangleClippingPointMaxNum))
                        {
                            assert(outputListCount < kTriangleClippingPointMaxNum);
                            continue;
                        }
                        outputList[outputListCount] = intersectingPoint;
                        outputListCount++;
                    }

                    // outputList.add(current_point);
                    assert(outputListCount < kTriangleClippingPointMaxNum);
                    if (!(outputListCount < kTriangleClippingPointMaxNum))
                    {
                        continue;
                    }
                    outputList[outputListCount] = currentPoint;
                    outputListCount++;
                }
                // prev_point inside clipEdge
                else if (0.0f <= d0)
                {
                    // Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
                    float t = d1 / (d1 - d0);
                    lerpVertex(&intersectingPoint, p1, p0, t);

                    // outputList.add(Intersecting_point);
                    assert(outputListCount < kTriangleClippingPointMaxNum);
                    if (!(outputListCount < kTriangleClippingPointMaxNum))
                    {
                        continue;
                    }
                    outputList[outputListCount] = intersectingPoint;
                    outputListCount++;
                }
            }
        }

        for (int j = 0; j < outputListCount; j++)
        {
            clippedPrimitiveVertices[j].clipSpacePosition = outputList[j].clipSpacePosition;
            for (int i = 0; i < _varyingVariableCount; i++)
            {
                clippedPrimitiveVertices[j].varyingVariables[i] = outputList[j].varyingVariables[i];
            }
        }
        *clippedPrimitiveVertiexCount = outputListCount;
    }

    void RasterizeStage::transformToNdcVertex(const Vertex* vertex, NdcVertex* ndcVertex) const
    {
        ndcVertex->ndcPosition = vertex->clipSpacePosition.getXYZ() / vertex->clipSpacePosition.w;
    }

    void RasterizeStage::transformRasterVertex(const Vertex* clippedPrimitiveVertex, const NdcVertex* ndcVertex, RasterVertex* rasterizationPoint) const
    {
#ifndef NDEBUG
        float lazyW = std::abs(clippedPrimitiveVertex->clipSpacePosition.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.x && clippedPrimitiveVertex->clipSpacePosition.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.y && clippedPrimitiveVertex->clipSpacePosition.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertex->clipSpacePosition.z && clippedPrimitiveVertex->clipSpacePosition.z <= lazyW);
#endif

        // 正規化デバイス座標からウィンドウ座標へ変換（ビューポート変換）
        // 
        // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glViewport.xml
        // 
        // ウィンドウ座標系(xy)
        // 
        //                         (x+w,y+h)
        //               +--------+
        //               |        |
        //               |        |
        //     +y        +--------+
        //       |  (x,y)
        //       |
        //       +--- +x
        //  (0,0)
        // 
        float halfWidth = (float)_viewportWidth / 2.0f;
        float halfHeight = (float)_viewportHeight / 2.0f;
        rasterizationPoint->wrcPosition.x = (ndcVertex->ndcPosition.x * halfWidth) + halfWidth + (float)_viewportX;
        rasterizationPoint->wrcPosition.y = (ndcVertex->ndcPosition.y * halfHeight) + halfHeight + (float)_viewportY;


        // 正規化デバイス座標(z)から深度に変換
        // 
        // see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glDepthRange.xml
        //
        float depthRangeNearVal = 0.0;
        float depthRangeFarVal = 1.0f;
        float t = (ndcVertex->ndcPosition.z + 1.0f) / 2.0f;
        rasterizationPoint->depth = lerp(depthRangeNearVal, depthRangeFarVal, t);

        // パースペクティブコレクト用の 1/W を保存
        float clipSpacePositionW = clippedPrimitiveVertex->clipSpacePosition.w;
        assert(clippedPrimitiveVertex->clipSpacePosition.w != 0.0f);
        rasterizationPoint->invW = 1.0f / clipSpacePositionW;
  
        //  パースペクティブコレクト用に補間変数をW除算
        for (int i = 0; i < _varyingVariableCount; i++)
        {
            rasterizationPoint->varyingVariablesDividedByW[i] = clippedPrimitiveVertex->varyingVariables[i] / clipSpacePositionW;
        }

    }

    void RasterizeStage::getLineFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, Fragment* fragment)
    {
        Vector2 a(p0->wrcPosition.x, p0->wrcPosition.y);
        Vector2 b(p1->wrcPosition.x, p1->wrcPosition.y);
        Vector2 c((float)x + 0.5f, (float)y + 0.5f);// ピクセルの中心
        Vector2 ab(b - a);
        Vector2 ac(c - a);
        float acLengthClosest = ab.normalize().dot(ac);

        float t = acLengthClosest / ab.getLength();
        t = clamp(t, 0.0f, 1.0f);

        // ２点間を補間
        Vector2 wrcPosition = Vector2::Lerp(p0->wrcPosition, p1->wrcPosition, t);
        float depth = lerp(p0->depth, p1->depth, t);
        float invW = lerp(p0->invW, p1->invW, t);

        // Varying 変数 も補間
        assert(0.0f != invW);
        float perspectiveCorrectW = 1.0f / invW;
        Vector4 varyingVariables[kMaxVaryingVectors] = {};
        for (int i = 0; i < _varyingVariableCount; i++)
        {
            const Vector4& v0 = p0->varyingVariablesDividedByW[i];
            const Vector4& v1 = p1->varyingVariablesDividedByW[i];
            varyingVariables[i] = Vector4::Lerp(v0, v1, t) * perspectiveCorrectW;
        }

        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < _varyingVariableCount; i++)
        {
            fragment->varyingVariables[i] = varyingVariables[i];
        }
        fragment->helperInvocation = false;
    }


    void RasterizeStage::getTriangleFragment(int x, int y, const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2, Fragment* fragment)
    {
        // see Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 

        Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

        // 重心座標の重みを求める（外積は三角形の面積の２倍）
        float area2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p2->wrcPosition);
        assert(0.0f != area2);
        float b0 = edgeFunction(p1->wrcPosition, p2->wrcPosition, p) / area2;
        float b1 = edgeFunction(p2->wrcPosition, p0->wrcPosition, p) / area2;
        float b2 = edgeFunction(p0->wrcPosition, p1->wrcPosition, p) / area2;

        // 補間
        Vector2 wrcPosition = (b0 * p0->wrcPosition) + (b1 * p1->wrcPosition) + (b2 * p2->wrcPosition);
        float depth = (b0 * p0->depth) + (b1 * p1->depth) + (b2 * p2->depth);
        float invW = (b0 * p0->invW) + (b1 * p1->invW) + (b2 * p2->invW);
        float perspectiveCorrectW;
#if 0
        assert(0.0f != invW);
#endif
        if (0.0f != invW)
        {
            perspectiveCorrectW = 1.0f / invW;
        }
        else
        {
            perspectiveCorrectW = INFINITY;
        }

        Vector4 varyingVariables[kMaxVaryingVectors] = {};
        for (int i = 0; i < _varyingVariableCount; i++)
        {
            const Vector4& v0 = p0->varyingVariablesDividedByW[i];
            const Vector4& v1 = p1->varyingVariablesDividedByW[i];
            const Vector4& v2 = p2->varyingVariablesDividedByW[i];
            varyingVariables[i] = ((b0 * v0) + (b1 * v1) + (b2 * v2)) * perspectiveCorrectW;
        }

        // ピクセルの中心を内外判定
        bool outside = false;
        if (b0 < 0.0f)
        {
            outside = true;
        }
        if (b1 < 0.0f)
        {
            outside = true;
        }
        if (b2 < 0.0f)
        {
            outside = true;
        }

        fragment->wrcPosition = wrcPosition;
        fragment->depth = depth;
        fragment->invW = invW;
        for (int i = 0; i < _varyingVariableCount; i++)
        {
            fragment->varyingVariables[i] = varyingVariables[i];
        }
        fragment->helperInvocation = outside;
    }

}
