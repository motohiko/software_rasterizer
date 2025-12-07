#include "ClipStage.h"
#include "..\Modules\Interpolator.h"
#include <cmath>// abs
#include <cassert>

namespace SoftwareRasterizer
{
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

    // クリップ境界座標系へ変換
    // ret : outside <= 0 < indide
    //
    // Blinn & Newell（1978）Clipping Using Homogeneous Coordinates - section 2. CLIPPING
    // SIGGRAPH '78: Proceedings of the 5th annual conference on Computer graphics and interactive techniques Pages 245 - 251
    //
    static float transformClippingBoundaryCoordinate(const Vector4& clipSpaceVertex, const ClippingPlaneParameter* clippingPlaneParameter)
    {
#if 0// TODO
        float clippingPoint = clipSpaceVertex.w * clippingPlaneParameter->sign;
        float offsetFromClippingPoint = clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex) - clippingPoint;
        // sign = 1.0f なら符号を反転
        return (clippingPlaneParameter->sign * -1.0f) * offsetFromClippingPoint;
#else
        return clipSpaceVertex.w - (clippingPlaneParameter->sign * clipSpaceVertex.getComponent(clippingPlaneParameter->vectorComponentIndex));
#endif
    }

    void ClipStage::setPrimitiveType(PrimitiveType primitiveType)
    {
        _primitiveType = primitiveType;
    }

    void ClipStage::clipPrimitive(VertexDataB** vertices, int vertexNum, VertexDataB* clippedVertices, int* clippedVertiexNum) const
	{
        switch (_primitiveType)
        {
        case PrimitiveType::kLine:
            clipPrimitiveLine(vertices, vertexNum, clippedVertices, clippedVertiexNum);
            break;
        case PrimitiveType::kTriangle:
            clipPrimitiveTriangle(vertices, vertexNum, clippedVertices, clippedVertiexNum);
            break;
        default:
            *clippedVertiexNum = 0;
            break;
        }
	}

    void ClipStage::setVaryingEnabledBits(const VaryingIndexState* varyingIndexState)
    {
        _varyingIndexState = varyingIndexState;
    }

    void ClipStage::clipPrimitiveLine(VertexDataB** primitiveVertices, int primitiveVertexCount, VertexDataB* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const
    {
        if (primitiveVertexCount != 2)
        {
            assert(primitiveVertexCount == 2);
            *clippedPrimitiveVertiexCount = 0;
            return;
        }

        for (int i = 0; i < 2; i++)
        {
            clippedPrimitiveVertices[i] = *primitiveVertices[i];
        }

        for (int i = 0; i < kClippingPlaneNum; i++)
        {
            // 境界座標系に変換
            float d0 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[0].clipCoord, &kClipPlaneParameters[i]);
            float d1 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[1].clipCoord, &kClipPlaneParameters[i]);
            if (0.0f < d0)
            {
                if (d1 < 0.0f)
                {
                    // d0: indide, d1: outside
                    const float t = d0 / (d0 - d1);
                    Interpolator::InterpolateLinear(&clippedPrimitiveVertices[1], &clippedPrimitiveVertices[0], &clippedPrimitiveVertices[1], t, _varyingIndexState);
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
                Interpolator::InterpolateLinear(&clippedPrimitiveVertices[0], &clippedPrimitiveVertices[0], &clippedPrimitiveVertices[1], t, _varyingIndexState);
            }
            else
            {
                // d0: outside, d1: outside
                return;
            }
        }

        *clippedPrimitiveVertiexCount = 2;

#ifndef NDEBUG
        if (false)
        {
            float lazyW = std::abs(clippedPrimitiveVertices[0].clipCoord.w) + 0.00001f;
            assert(-lazyW <= clippedPrimitiveVertices[0].clipCoord.x && clippedPrimitiveVertices[0].clipCoord.x <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertices[0].clipCoord.y && clippedPrimitiveVertices[0].clipCoord.y <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertices[0].clipCoord.z && clippedPrimitiveVertices[0].clipCoord.z <= lazyW);

            lazyW = std::abs(clippedPrimitiveVertices[1].clipCoord.w) + 0.00001f;
            assert(-lazyW <= clippedPrimitiveVertices[1].clipCoord.x && clippedPrimitiveVertices[1].clipCoord.x <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertices[1].clipCoord.y && clippedPrimitiveVertices[1].clipCoord.y <= lazyW);
            assert(-lazyW <= clippedPrimitiveVertices[1].clipCoord.z && clippedPrimitiveVertices[1].clipCoord.z <= lazyW);
        }
#endif
    }

    void ClipStage::clipPrimitiveTriangle(VertexDataB** primitiveVertices, int primitiveVertexCount, VertexDataB* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount) const
    {
        // Sutherland-Hodgman algorithm
        // Ivan Sutherland, Gary W. Hodgman: Reentrant Polygon Clipping. Communications of the ACM, vol. 17, pp. 32-42, 1974
        //
        // 参考 https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
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
            *clippedPrimitiveVertiexCount = 0;
            return;
        }

        VertexDataB inputList[kClippingPointMaxNum] = {};
        int inputListCount = 0;

        VertexDataB outputList[kClippingPointMaxNum] = {};
        int outputListCount = 0;

        // List outputList = subjectPolygon;
        for (int i = 0; i < 3; i++)
        {
            outputList[outputListCount] = *primitiveVertices[i];
            outputListCount++;
        }

        //for (Edge clipEdge in clipPolygon) do
        for (int i = 0; i < kClippingPlaneNum; i++)
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
                VertexDataB& currentPoint = inputList[j];
                VertexDataB& prevPoint = inputList[((j - 1) + inputListCount) % inputListCount];// (0 - 1) % n = -1 になるので、 n を足してから余剰を求める

                VertexDataB& p0 = prevPoint;
                VertexDataB& p1 = currentPoint;

                // 境界座標系に変換（0 <= d のとき indide）
                float d0 = transformClippingBoundaryCoordinate(p0.clipCoord, &kClipPlaneParameters[i]);
                float d1 = transformClippingBoundaryCoordinate(p1.clipCoord, &kClipPlaneParameters[i]);

                VertexDataB intersectingPoint;

                // current_point inside clipEdge
                if (0.0f <= d1)
                {
                    // prev_point not inside clipEdge
                    if (d0 < 0.0f)
                    {
                        // Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
                        float t = d1 / (d1 - d0);
                        Interpolator::InterpolateLinear(&intersectingPoint, &p1, &p0, t, _varyingIndexState);

                        // outputList.add(Intersecting_point);
                        if (!(outputListCount < kClippingPointMaxNum))
                        {
                            assert(outputListCount < kClippingPointMaxNum);
                            continue;
                        }
                        outputList[outputListCount] = intersectingPoint;
                        outputListCount++;
                    }

                    // outputList.add(current_point);
                    assert(outputListCount < kClippingPointMaxNum);
                    if (!(outputListCount < kClippingPointMaxNum))
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
                    Interpolator::InterpolateLinear(&intersectingPoint, &p1, &p0, t, _varyingIndexState);

                    // outputList.add(Intersecting_point);
                    assert(outputListCount < kClippingPointMaxNum);
                    if (!(outputListCount < kClippingPointMaxNum))
                    {
                        continue;
                    }
                    outputList[outputListCount] = intersectingPoint;
                    outputListCount++;
                }
            }
        }

        for (int i = 0; i < outputListCount; i++)
        {
            clippedPrimitiveVertices[i] = outputList[i];
        }
        *clippedPrimitiveVertiexCount = outputListCount;
    }
}
