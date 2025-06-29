#include "Renderer.h"

#include <cstdint>
#include <memory>// memset
#include <algorithm>// fill

//# include <omp.h>

namespace MyApp
{
    void Renderer::clearFrameBuffer()
    {
        // glClear()

        // glClearColor(r, g, b, a) (Default all 0)
        size_t size = _colorFrameBuffer->widthBytes * _colorFrameBuffer->height;
        std::memset(_colorFrameBuffer->addr, 0, size);
        //uint32_t* firstColor = (uint32_t*)_colorFrameBuffer->addr;
        //uint32_t* lastColor = ((uint32_t*)_colorFrameBuffer->addr) + (_colorFrameBuffer->height * _colorFrameBuffer->width);
        //std::fill(firstColor, lastColor, 0x00000000);

        // glClearDepth(depth) (Default 1)
        float* firstDepth = (float*)_depthFrameBuffer->addr;
        float* lastDepth = ((float*)_depthFrameBuffer->addr) + (_depthFrameBuffer->height * _depthFrameBuffer->width);
        std::fill(firstDepth, lastDepth, 1.0f);
    }


    void Renderer::drawIndexed()
    {
        VertexShaderOutput vertexShaderOutputs[3] = {};
        int vertexShaderOutputCount = 0;

        for (int i = 0; i < _indexBufferIndexCount; ++i)
        {
            uint16_t index = _indexBufferIndices[i];

            {
                //
                // Input Assembly stage
                //

                VertexShaderInput vertexShaderInput = {};
                vertexShaderInput.uniformBlock = _uniformBlock;

                inputAssembly(index, vertexShaderInput.elements);

                //
                // Vertex Shader stage
                // (transform clipping coordinates)
                //

                assert(vertexShaderOutputCount < std::size(vertexShaderOutputs));
                VertexShaderOutput* vertexShaderOutput = &(vertexShaderOutputs[vertexShaderOutputCount]);
                vertexShaderOutputCount++;

                _vertexShaderMainFunc(&vertexShaderInput, vertexShaderOutput);
            }

            switch (_primitiveTopologyType)
            {
            case PrimitiveTopologyType::LineList:
                if (2 == vertexShaderOutputCount)
                {
                    outputPrimitiveLine(vertexShaderOutputs, 2);
                    vertexShaderOutputCount = 0;
                }
                break;
            case PrimitiveTopologyType::TriangleList:
                if (3 == vertexShaderOutputCount)
                {
                    outputPrimitiveTriangle(vertexShaderOutputs, 3);
                    vertexShaderOutputCount = 0;
                }
                break;
            }
        }
    }

    void Renderer::inputAssembly(int index, Vector4* elements)
    {
        for (int i = 0; i < _vertexBufferElementCount; ++i)
        {
            if (nullptr == _vertexElements[i].buffer)
            {
                continue;
            }
            const float* src = (const float*)((uintptr_t)(_vertexElements[i].buffer) + _vertexElements[i].strideInBytes * index);
            switch (_vertexElements[i].strideInBytes)
            {
            case 8:
                elements[i] = Vector4(src[0], src[1], 0.0f, 1.0f);// TODO w=0.0 ?
                break;
            case 12:
                elements[i] = Vector4(src[0], src[1], src[2], 1.0f);// TODO w=0.0 ?
                break;
            case 16:
                elements[i] = Vector4(src[0], src[1], src[2], src[3]);
                break;
            }
        }
    }

    void Renderer::outputPrimitiveLine(const VertexShaderOutput* vertexShaderOutputs, int vertexShaderOutputCount)
    {
        assert(vertexShaderOutputCount == 2);
        if (vertexShaderOutputCount != 2)
        {
            return;
        }

        //
        // Clipping
        //

        ClippedPrimitiveVertex clippedPrimitiveVertices[2];
        int clippedPrimitiveVertexCount = 0;

        clipPrimitiveLine(vertexShaderOutputs, vertexShaderOutputCount, clippedPrimitiveVertices, &clippedPrimitiveVertexCount);
        if (clippedPrimitiveVertexCount != 2)
        {
            return;
        }


        //
        // Rasterize stage
        //

        RasterizationPopint rasterizationPopint[2] = {};

        for (int i = 0; i < 2; ++i)
        {
            makeRasterizationPopint(&clippedPrimitiveVertices[i], &rasterizationPopint[i]);
        }

        rasterizeLine(&rasterizationPopint[0], &rasterizationPopint[1]);
    }

    void Renderer::outputPrimitiveTriangle(const VertexShaderOutput* vertexShaderOutputs, int vertexShaderOutputCount)
    {
        assert(vertexShaderOutputCount == 3);
        if (vertexShaderOutputCount != 3)
        {
            return;
        }

        //
        // Clipping
        //

        ClippedPrimitiveVertex clippedPrimitiveVertices[kTriangleClippingPointMaxNum];
        int clippedPrimitiveVertiexCount = 0;

        clipPrimitiveTriangle(vertexShaderOutputs, vertexShaderOutputCount, clippedPrimitiveVertices, &clippedPrimitiveVertiexCount);
        if (clippedPrimitiveVertiexCount < 3)
        {
            return;
        }


        //
        // Face Culling
        //

        {
            // glFrontFace(GL_CCW) // OpenGL default
            // glEnable(GL_CULL_FACE)

            // 先頭の三角形で判定
            // ※クリップ処理後は多角形の可能性がある
            const Vector2 ndcPositions[3] =
            {
                clippedPrimitiveVertices[0].position.getXY() / clippedPrimitiveVertices[0].position.w,
                clippedPrimitiveVertices[1].position.getXY() / clippedPrimitiveVertices[1].position.w,
                clippedPrimitiveVertices[2].position.getXY() / clippedPrimitiveVertices[2].position.w,
            };
            const Vector2& p0 = ndcPositions[0];
            const Vector2& p1 = ndcPositions[1];
            const Vector2& p2 = ndcPositions[2];

            // TODO
            float n = (p1 - p0).cross(p2 - p0);// CCW
            if (n <= 0.0f)// GL_CULL_FACE
            {
                return;
            }
        }


        //
        // Rasterize stage
        //

        RasterizationPopint rasterizationPoints[kTriangleClippingPointMaxNum] = {};
        int rasterizationPointsCount = clippedPrimitiveVertiexCount;

        for (int i = 0; i < clippedPrimitiveVertiexCount; ++i)
        {
            makeRasterizationPopint(&clippedPrimitiveVertices[i], &rasterizationPoints[i]);
        }

        for (int i = 0; i < rasterizationPointsCount - 2; ++i)
        {
            const RasterizationPopint* p0 = &rasterizationPoints[0];// 0 固定
            const RasterizationPopint* p1 = &rasterizationPoints[i + 1];
            const RasterizationPopint* p2 = &rasterizationPoints[i + 2];

            rasterizeTriangleFaceOnly(p0, p1, p2);// CCW
            rasterizeTriangleFaceOnly(p2, p1, p0);// CW

            // debug code
            if (false)
            {
                rasterizeLine(p0, p1);
                rasterizeLine(p1, p2);
                rasterizeLine(p2, p0);
            }
        }
    }

    void Renderer::clipPrimitiveLine(const VertexShaderOutput* primitiveVertices, int primitiveVertexCount, ClippedPrimitiveVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount)
    {
        assert(primitiveVertexCount == 2);
        if (primitiveVertexCount != 2)
        {
            return;
        }

        for (int j = 0; j < 2; ++j)
        {
            clippedPrimitiveVertices[j].position = primitiveVertices[j].position;
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                clippedPrimitiveVertices[j].varyingVariables[i] = primitiveVertices[j].varyingVariables[i];
            }
        }

        for (int i = 0; i < kClippingPlaneNum; ++i)
        {
            // 境界座標系に変換
            float d0 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[0].position, &kClippingPlaneParameters[i]);
            float d1 = transformClippingBoundaryCoordinate(clippedPrimitiveVertices[1].position, &kClippingPlaneParameters[i]);
            if (0.0f < d0)
            {
                if (d1 < 0.0f)
                {
                    // d0: indide, d1: outside
                    const float t = d0 / (d0 - d1);
                    lerpPrimitiveVertex(&clippedPrimitiveVertices[1], clippedPrimitiveVertices[0], clippedPrimitiveVertices[1], t);
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
                lerpPrimitiveVertex(&clippedPrimitiveVertices[0], clippedPrimitiveVertices[0], clippedPrimitiveVertices[1], t);
            }
            else
            {
                // d0: outside, d1: outside
                return;
            }
        }

        *clippedPrimitiveVertiexCount = 2;

#ifndef NDEBUG
        float lazyW = std::abs(clippedPrimitiveVertices[0].position.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertices[0].position.x && clippedPrimitiveVertices[0].position.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[0].position.y && clippedPrimitiveVertices[0].position.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[0].position.z && clippedPrimitiveVertices[0].position.z <= lazyW);

        lazyW = std::abs(clippedPrimitiveVertices[1].position.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertices[1].position.x && clippedPrimitiveVertices[1].position.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[1].position.y && clippedPrimitiveVertices[1].position.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices[1].position.z && clippedPrimitiveVertices[1].position.z <= lazyW);
#endif
    }

    void Renderer::clipPrimitiveTriangle(const VertexShaderOutput* primitiveVertices, int primitiveVertexCount, ClippedPrimitiveVertex* clippedPrimitiveVertices, int* clippedPrimitiveVertiexCount)
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

        ClippedPrimitiveVertex inputList[kTriangleClippingPointMaxNum] = {};
        int inputListCount = 0;

        ClippedPrimitiveVertex outputList[kTriangleClippingPointMaxNum] = {};
        int outputListCount = 0;

        // List outputList = subjectPolygon;
        for (int j = 0; j < 3; ++j)
        {
            outputList[outputListCount].position = primitiveVertices[j].position;
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                outputList[outputListCount].varyingVariables[i] = primitiveVertices[j].varyingVariables[i];
            }
            outputListCount++;
        }

        //for (Edge clipEdge in clipPolygon) do
        for (int k = 0; k < kClippingPlaneNum; ++k)
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
            for (int j = 0; j < inputListCount; ++j)
            {
                // Point current_point = inputList[i];
                // Point prev_point = inputList[(i - 1) % inputList.count];
                ClippedPrimitiveVertex& currentPoint = inputList[j];
                ClippedPrimitiveVertex& prevPoint = inputList[((j - 1) + inputListCount) % inputListCount];// (0 - 1) % n = -1 になるので、 n を足してから余剰を求める

                ClippedPrimitiveVertex& p0 = prevPoint;
                ClippedPrimitiveVertex& p1 = currentPoint;

                // 境界座標系に変換（0 <= d のとき indide）
                float d0 = transformClippingBoundaryCoordinate(p0.position, &kClippingPlaneParameters[k]);
                float d1 = transformClippingBoundaryCoordinate(p1.position, &kClippingPlaneParameters[k]);

                ClippedPrimitiveVertex intersectingPoint;

                // current_point inside clipEdge
                if (0.0f <= d1)
                {
                    // prev_point not inside clipEdge
                    if (d0 < 0.0f)
                    {
                        // Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
                        float t = d1 / (d1 - d0);
                        lerpPrimitiveVertex(&intersectingPoint, p1, p0, t);

                        // outputList.add(Intersecting_point);
                        assert(outputListCount < kTriangleClippingPointMaxNum);
                        if (!(outputListCount < kTriangleClippingPointMaxNum))
                        {
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
                    lerpPrimitiveVertex(&intersectingPoint, p1, p0, t);

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

        for (int j = 0; j < outputListCount; ++j)
        {
            clippedPrimitiveVertices[j].position = outputList[j].position;
            for (int i = 0; i < _varyingVariableCount; ++i)
            {
                clippedPrimitiveVertices[j].varyingVariables[i] = outputList[j].varyingVariables[i];
            }
        }
        *clippedPrimitiveVertiexCount = outputListCount;
    }

    void Renderer::makeRasterizationPopint(const ClippedPrimitiveVertex* clippedPrimitiveVertices, RasterizationPopint* rasterizationPopint)
    {
#ifndef NDEBUG
        float lazyW = std::abs(clippedPrimitiveVertices->position.w) + 0.00001f;
        assert(-lazyW <= clippedPrimitiveVertices->position.x && clippedPrimitiveVertices->position.x <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices->position.y && clippedPrimitiveVertices->position.y <= lazyW);
        assert(-lazyW <= clippedPrimitiveVertices->position.z && clippedPrimitiveVertices->position.z <= lazyW);
#endif

        assert(clippedPrimitiveVertices->position.w != 0.0f);

        // 正規化デバイス座標へ変換（透視除算またはW除算）
        Vector4 ndcPosition = clippedPrimitiveVertices->position / clippedPrimitiveVertices->position.w;
        // ※誤差で捕まるのでassertはコメントアウト
        //assert(-1.0f <= ndcPosition.x && ndcPosition.x <= 1.0f);
        //assert(-1.0f <= ndcPosition.y && ndcPosition.y <= 1.0f);
        //assert(-1.0f <= ndcPosition.z && ndcPosition.z <= 1.0f);
        rasterizationPopint->ndcPosition = ndcPosition;

        // ビューポート変換
        rasterizationPopint->screenPosition.x = (ndcPosition.x + 1.0f) * ((float)(_viewportWidth) / 2.0f);
        rasterizationPopint->screenPosition.y = (ndcPosition.y + 1.0f) * ((float)(_viewportHeight) / 2.0f);

        // 補間値のパースペクティブコレクト仕込み
        float perspectiveCorrectW = clippedPrimitiveVertices->position.w;
        for (int i = 0; i < _varyingVariableCount; ++i)
        {
            rasterizationPopint->varyingVariablesDividedByW[i] = clippedPrimitiveVertices->varyingVariables[i] / perspectiveCorrectW;
        }
        rasterizationPopint->perspectiveCorrectInvW = 1.0f / perspectiveCorrectW;
    }

    void Renderer::rasterizeLine(const RasterizationPopint* rasterizationPopint0, const RasterizationPopint* rasterizationPopint1)
    {
        const RasterizationPopint* p0 = rasterizationPopint0;
        const RasterizationPopint* p1 = rasterizationPopint1;

        int x0 = (int)std::floor(p0->screenPosition.x);// 切り捨て
        int y0 = (int)std::floor(p0->screenPosition.y);
        int x1 = (int)std::floor(p1->screenPosition.x);
        int y1 = (int)std::floor(p1->screenPosition.y);

        //
        // Bresenham's line algorithm
        //
        int dstX = x1 - x0;
        int dstY = y1 - y0;
        int addX = 1;
        int addY = 1;
        if (dstX < 0)
        {
            dstX = -dstX;
            addX = -1;
        }
        if (dstY < 0)
        {
            dstY = -dstY;
            addY = -1;
        }

        int ctr = 0;
        int x = x0;
        int y = y0;
        if (dstX >= dstY)
        {
            for (int i = 0; i < dstX; ++i)
            {
                rasterizeLinePixel(p0, p1, x, y);

                x += addX;
                ctr += dstY;
                if (ctr >= dstX)
                {
                    y += addY;
                    ctr -= dstX;
                }
            }
        }
        else
        {
            for (int i = 0; i < dstY; ++i)
            {
                rasterizeLinePixel(p0, p1, x, y);

                y += addY;
                ctr += dstX;
                if (ctr >= dstY)
                {
                    x += addX;
                    ctr -= dstY;
                }
            }
        }
    }

    void Renderer::rasterizeLinePixel(const RasterizationPopint* rasterizationPopint0, const RasterizationPopint* rasterizationPopint1, int x, int y)
    {
        const RasterizationPopint* p0 = rasterizationPopint0;
        const RasterizationPopint* p1 = rasterizationPopint1;

        int width = _colorFrameBuffer->width;
        int height = _colorFrameBuffer->height;
        if (x < 0 || width <= x || y < 0 || height <= y)
        {
            return;
        }

        Vector2 a(p0->screenPosition.x, p0->screenPosition.y);
        Vector2 b(p1->screenPosition.x, p1->screenPosition.y);
        Vector2 c((float)x + 0.5f, (float)y + 0.5f);// ピクセルの中心
        Vector2 ab = (b - a);
        Vector2 ac = (c - a);
        float acLengthClosest = ab.normalize().dot(ac);

        float t = acLengthClosest / ab.getLength();
        t = clamp(t, 0.0f, 1.0f);


        //
        // Early Fragment Test (Early Depth Test) 
        //

        float z = lerp(p0->ndcPosition.z, p1->ndcPosition.z, t);

        // glDepthRange(nearVal, farVal) (Default 0, 1)
        float depth = remapDepth(z, -1.0f, 1.0f, 0.0f, 1.0f);

        bool pass = depthTest(x, y, depth);
        if (!pass)
        {
            return;
        }


        //
        // 　補間値を求める
        //

        float perspectiveCorrectInvW = lerp(p0->perspectiveCorrectInvW, p1->perspectiveCorrectInvW, t);
        assert(0.0f != perspectiveCorrectInvW);
        float perspectiveCorrectW = 1.0f / perspectiveCorrectInvW;

        Vector4 interpolators[kVaryingVariableNum] = {};
        for (int i = 0; i < _varyingVariableCount; ++i)
        {
            const Vector4& v0 = p0->varyingVariablesDividedByW[i];
            const Vector4& v1 = p1->varyingVariablesDividedByW[i];
            interpolators[i] = Vector4::Lerp(v0, v1, t) * perspectiveCorrectW;
        }

        //
        // pixel shader stage
        //

        PixelShaderInput input = {};
        input.uniformBlock = _uniformBlock;
        input.varyingVariables = interpolators;

        PixelShaderOutput output = {};

        _pixelShaderMainFunc(&input, &output);

        //
        // color blend stage ?
        //

        setPixel(x, y, output.fragColor, depth);
    }

    void Renderer::rasterizeTriangleFaceOnly(const RasterizationPopint* rasterizationPopint0, const RasterizationPopint* rasterizationPopint1, const RasterizationPopint* rasterizationPopint2)
    {
        //
        // see Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 
        //

        const RasterizationPopint* p0 = rasterizationPopint0;
        const RasterizationPopint* p1 = rasterizationPopint1;
        const RasterizationPopint* p2 = rasterizationPopint2;

        // 三角形の表裏判定
        const float a = edgeFunction(p0->screenPosition, p1->screenPosition, p2->screenPosition);
        if (a <= 0.0f)
        {
            return;
        }

        // 処理が重すぎるので範囲を絞り込む
        struct BoundingBox2d
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
            void addPoint(const Vector2& p)
            {
                minX = std::min(minX, p.x);
                minY = std::min(minY, p.y);
                maxX = std::max(maxX, p.x);
                maxY = std::max(maxY, p.y);
            }
        };
        BoundingBox2d boundingBox = {};
        boundingBox.Init();
        boundingBox.addPoint(p0->screenPosition);
        boundingBox.addPoint(p1->screenPosition);
        boundingBox.addPoint(p2->screenPosition);
        int minX = (int)std::floor(boundingBox.minX);// 切り捨て
        int minY = (int)std::floor(boundingBox.minY);
        int maxX = (int)std::ceil(boundingBox.maxX);// 切り上げ
        int maxY = (int)std::ceil(boundingBox.maxY);
        int width = _colorFrameBuffer->width;
        int height = _colorFrameBuffer->height;
        minX = clamp(minX, 0, width - 1);
        minY = clamp(minY, 0, height - 1);
        maxX = clamp(maxX, 0, width - 1);
        maxY = clamp(maxY, 0, height - 1);

        //#pragma omp parallel for
        for (int y = minY; y <= maxY; ++y)
        {
            //#pragma omp parallel for
            for (int x = minX; x <= maxX; ++x)
            {
                Vector2 p(x + 0.5f, y + 0.5f);// ピクセルの中心

                // ピクセルの中心を内外判定
                float b0 = edgeFunction(p1->screenPosition, p2->screenPosition, p);
                if (b0 < 0.0f)
                {
                    continue;
                }
                float b1 = edgeFunction(p2->screenPosition, p0->screenPosition, p);
                if (b1 < 0.0f)
                {
                    continue;
                }
                float b2 = edgeFunction(p0->screenPosition, p1->screenPosition, p);
                if (b2 < 0.0f)
                {
                    continue;
                }

                // 重心座標の重みを求める
                // ※edgeFunction（外積）は三角形の面積の２倍を返す
                b0 /= a;
                b1 /= a;
                b2 /= a;


                //
                // Early Fragment Test (Early Depth Test) 
                //

                float z = ((b0 * p0->ndcPosition.z) + (b1 * p1->ndcPosition.z) + (b2 * p2->ndcPosition.z));

                // glDepthRange(nearVal, farVal) (Default 0, 1)
                float depth = remapDepth(z, -1.0f, 1.0f, 0.0f, 1.0f);

                bool pass = depthTest(x, y, depth);
                if (!pass)
                {
                    continue;
                }


                //
                // 補間値を求める
                //
                
                float invW0 = p0->perspectiveCorrectInvW;
                float invW1 = p1->perspectiveCorrectInvW;
                float invW2 = p2->perspectiveCorrectInvW;
                float perspectiveCorrectInvW = ((b0 * invW0) + (b1 * invW1) + (b2 * invW2));
                assert(0.0f != perspectiveCorrectInvW);
                float perspectiveCorrectW = 1.0f / perspectiveCorrectInvW;

                Vector4 varyingVariables[kVaryingVariableNum] = {};
                for (int i = 0; i < _varyingVariableCount; ++i)
                {
                    const Vector4& v0 = p0->varyingVariablesDividedByW[i];
                    const Vector4& v1 = p1->varyingVariablesDividedByW[i];
                    const Vector4& v2 = p2->varyingVariablesDividedByW[i];
                    varyingVariables[i] = ((b0 * v0) + (b1 * v1) + (b2 * v2)) * perspectiveCorrectW;
                }

                //
                // pixel shader stage
                //

                PixelShaderInput input = {};
                input.uniformBlock = _uniformBlock;
                input.varyingVariables = varyingVariables;

                PixelShaderOutput output = {};

                _pixelShaderMainFunc(&input, &output);

                //
                // color blend stage ?
                //

                setPixel(x, y, output.fragColor, depth);
            }
        }
    }
}
