#include "Renderer.h"

#include <cstdint>
#include <memory>// memset
#include <algorithm>// fill


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

    void Renderer::InputAssembly(int index, Vector4* elements)
    {
        for (int i = 0; i < _vertexBufferElementsCount; ++i)
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


    // メモ：（クリップ空間座標系の）w はソースの z（を符号反転したもの）が入っているので、xyz と一緒に線形補間する

    void Renderer::clipPrimitiveLine(const VertexShaderOutput* primitiveVertices, ClippingPoint* clippedPoints, int* clippingPointsCount)
    {
        for (int j = 0; j < 2; ++j)
        {
            clippedPoints[j].position = primitiveVertices[j].position;
            for (int i = 0; i < _varyingVariablesCount; ++i)
            {
                clippedPoints[j].varyingVariables[i] = primitiveVertices[j].varyingVariables[i];
            }
        }

        for (int k = 0; k < kClippingPlaneNum; ++k)
        {
            // 境界座標系に変換
            float d0 = transformClippingBoundaryCoordinate(clippedPoints[0].position, &kClippingPlaneParameters[k]);
            float d1 = transformClippingBoundaryCoordinate(clippedPoints[1].position, &kClippingPlaneParameters[k]);
            if (0.0f < d0)
            {
                if (d1 < 0.0f)
                {
                    // d0: indide, d1: outside
                    const float t = d0 / (d0 - d1);
                    lerpClippingPoint(&clippedPoints[1], clippedPoints[0], clippedPoints[1], t);
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
                lerpClippingPoint(&clippedPoints[0], clippedPoints[0], clippedPoints[1], t);
            }
            else
            {
                // d0: outside, d1: outside
                return;
            }
        }

        *clippingPointsCount = 2;

#ifndef NDEBUG
        //float lazyW = std::abs(clippedPoints[0].position.w) + 0.00001f;
        //assert(-lazyW <= clippedPoints[0].position.x && clippedPoints[0].position.x <= lazyW);
        //assert(-lazyW <= clippedPoints[0].position.y && clippedPoints[0].position.y <= lazyW);
        //assert(-lazyW <= clippedPoints[0].position.z && clippedPoints[0].position.z <= lazyW);
#endif

#ifndef NDEBUG
        //lazyW = std::abs(clippedPoints[1].position.w) + 0.00001f;
        //assert(-lazyW <= clippedPoints[1].position.x && clippedPoints[1].position.x <= lazyW);
        //assert(-lazyW <= clippedPoints[1].position.y && clippedPoints[1].position.y <= lazyW);
        //assert(-lazyW <= clippedPoints[1].position.z && clippedPoints[1].position.z <= lazyW);
#endif
    }

    void Renderer::clipPrimitiveTriangle(const VertexShaderOutput* primitiveVertices, ClippingPoint* clippingPoints, int* clippingPointsCount)
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

        ClippingPoint inputList[kTriangleClippingPointMaxNum] = {};
        int inputListCount = 0;

        ClippingPoint outputList[kTriangleClippingPointMaxNum] = {};
        int outputListCount = 0;

        // List outputList = subjectPolygon;
        for (int j = 0; j < 3; ++j)
        {
            outputList[outputListCount].position = primitiveVertices[j].position;
            for (int i = 0; i < _varyingVariablesCount; ++i)
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
                ClippingPoint& currentPoint = inputList[j];
                ClippingPoint& prevPoint = inputList[((j - 1) + inputListCount) % inputListCount];// (0 - 1) % n = -1 になるので、 n を足してから余剰を求める

                ClippingPoint& p0 = prevPoint;
                ClippingPoint& p1 = currentPoint;

                // 境界座標系に変換（0 <= d のとき indide）
                float d0 = transformClippingBoundaryCoordinate(p0.position, &kClippingPlaneParameters[k]);
                float d1 = transformClippingBoundaryCoordinate(p1.position, &kClippingPlaneParameters[k]);

                ClippingPoint intersectingPoint;

                // current_point inside clipEdge
                if (0.0f <= d1)
                {
                    // prev_point not inside clipEdge
                    if (d0 < 0.0f)
                    {
                        // Point Intersecting_point = ComputeIntersection(prev_point, current_point, clipEdge)
                        float t = d1 / (d1 - d0);
                        lerpClippingPoint(&intersectingPoint, p1, p0, t);

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
                    lerpClippingPoint(&intersectingPoint, p1, p0, t);

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
            clippingPoints[j].position = outputList[j].position;
            for (int i = 0; i < _varyingVariablesCount; ++i)
            {
                clippingPoints[j].varyingVariables[i] = outputList[j].varyingVariables[i];
            }
        }
        *clippingPointsCount = outputListCount;
    }

    void Renderer::rasterizeLinePixel(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1, int x, int y)
    {
        int width = _colorFrameBuffer->width;
        int height = _colorFrameBuffer->height;
        if (x < 0 || width <= x || y < 0 || height <= y)
        {
            return;
        }

        const Vector4& p0 = screenPoint0->position;
        const Vector4& p1 = screenPoint1->position;

        Vector2 a(p0.x, p0.y);
        Vector2 b(p1.x, p1.y);
        Vector2 c((float)x + 0.5f, (float)y + 0.5f);// ピクセルの中心
        Vector2 ab = (b - a);
        Vector2 ac = (c - a);
        float acLengthClosest = ab.normalize().dot(ac);

        float t = acLengthClosest / ab.getLength();
        t = clamp(t, 0.0f, 1.0f);

        float ndcZ = lerp(p0.z, p1.z, t);

        // glDepthRange(nearVal, farVal) (Default 0, 1)
        float depth = remapDepth(ndcZ, -1.0f, 1.0f, 0.0f, 1.0f);

        //
        // Early Fragment Test (Early Depth Test) 
        //
        bool pass = depthTest(x, y, depth);
        if (!pass)
        {
            return;
        }

        float invW = lerp(p0.w, p1.w, t);
        assert(0.0f != invW);
        float w = 1.0f / invW;

        // 　補間値も求める
        Vector4 interpolators[kVaryingVariablesNum] = {};
        for (int i = 0; i < _varyingVariablesCount; ++i)
        {
            interpolators[i] = Vector4::Lerp(screenPoint0->varyingVariables[i], screenPoint1->varyingVariables[i], t);
#ifdef TEST_PERSPECTIVE_CORRECT_VARYING_VARIABLES
            interpolators[i] *= w;
#endif
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

    void Renderer::rasterizeLine(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1)
    {
        const Vector4& p0 = screenPoint0->position;
        const Vector4& p1 = screenPoint1->position;

        int x0 = (int)std::floor(p0.x);// 切り捨て
        int y0 = (int)std::floor(p0.y);
        int x1 = (int)std::floor(p1.x);
        int y1 = (int)std::floor(p1.y);

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
                rasterizeLinePixel(screenPoint0, screenPoint1, x, y);

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
                rasterizeLinePixel(screenPoint0, screenPoint1, x, y);

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

    void Renderer::rasterizeTriangle(const ScreenPoint* screenPoint0, const ScreenPoint* screenPoint1, const ScreenPoint* screenPoint2)
    {
        const Vector4& p0 = screenPoint0->position;
        const Vector4& p1 = screenPoint1->position;
        const Vector4& p2 = screenPoint2->position;

        const auto Denom = edgeFunction(p0.GetXY(), p1.GetXY(), p2.GetXY());// Denom は denominator らしい、変えたほうがよい
        if (Denom <= 0.0f)
        {
            return;
        }

        // 処理が重すぎるので範囲を絞り込む
        BoundingBox boundingBox = {};
        boundingBox.Init();
        boundingBox.addPoint(screenPoint0->position);
        boundingBox.addPoint(screenPoint1->position);
        boundingBox.addPoint(screenPoint2->position);

        int minX = (int)std::floor(boundingBox.minX);// 切り捨て
        int minY = (int)std::floor(boundingBox.minY);
        int maxX = (int)std::ceil(boundingBox.maxX);// 切り上げ
        int maxY = (int)std::ceil(boundingBox.maxY);

        // clipping
        int width = _colorFrameBuffer->width;
        int height = _colorFrameBuffer->height;
        minX = clamp(minX, 0, width - 1);
        minY = clamp(minY, 0, height - 1);
        maxX = clamp(maxX, 0, width - 1);
        maxY = clamp(maxY, 0, height - 1);

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                Vector2 p(x + 0.5f, y + 0.5f);

                // ピクセルの中心を内外判定
                auto b0 = edgeFunction(p1.GetXY(), p2.GetXY(), p);
                if (b0 < 0.0f) continue;
                auto b1 = edgeFunction(p2.GetXY(), p0.GetXY(), p);
                if (b1 < 0.0f) continue;
                auto b2 = edgeFunction(p0.GetXY(), p1.GetXY(), p);
                if (b2 < 0.0f) continue;

                // 重心座標
                b0 /= Denom;
                b1 /= Denom;
                b2 /= Denom;

                // 補間値を重心座標で求める

                float ndcZ = ((b0 * p0.z) + (b1 * p1.z) + (b2 * p2.z));

                // glDepthRange(nearVal, farVal) (Default 0, 1)
                float depth = remapDepth(ndcZ, -1.0f, 1.0f, 0.0f, 1.0f);


                //
                // Early Fragment Test (Early Depth Test) 
                //

                bool pass = depthTest(x, y, depth);
                if (!pass)
                {
                    continue;
                }

                float invW = ((b0 * p0.w) + (b1 * p1.w) + (b2 * p2.w));
                assert(0.0f != invW);
                float w = 1.0f / invW;

                // 残りの補間値も求める
                Vector4 varyingVariables[kVaryingVariablesNum] = {};
                for (int i = 0; i < _varyingVariablesCount; ++i)
                {
                    const Vector4& v0 = screenPoint0->varyingVariables[i];
                    const Vector4& v1 = screenPoint1->varyingVariables[i];
                    const Vector4& v2 = screenPoint2->varyingVariables[i];
                    varyingVariables[i] = ((b0 * v0) + (b1 * v1) + (b2 * v2));
#ifdef TEST_PERSPECTIVE_CORRECT_VARYING_VARIABLES
                    varyingVariables[i] *= w;
#endif
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

    void Renderer::outputPrimitiveLine(const VertexShaderOutput* vertexShaderOutputs)
    {
        //
        // Clipping stage
        //

        ClippingPoint clippingPoints[2];
        int clippingPointsCount = 0;
        clipPrimitiveLine(vertexShaderOutputs, clippingPoints, &clippingPointsCount);
        if (clippingPointsCount != 2)
        {
            return;
        }

        // NDC
        // VIEWPORT

        ScreenPoint screenPoints[2] = {};
        transformScreenCoordinates(*this, &clippingPoints[0], &screenPoints[0]);
        transformScreenCoordinates(*this, &clippingPoints[1], &screenPoints[1]);

        //
        // Rasterize stage
        //

        rasterizeLine(&screenPoints[0], &screenPoints[1]);
    }

    void Renderer::outputPrimitiveTriangle(const VertexShaderOutput* vertexShaderOutputs)
    {
        //
        // Clipping stage
        //

        ClippingPoint clippingPoints[kTriangleClippingPointMaxNum];
        int clippingPointsCount = 0;
        clipPrimitiveTriangle(vertexShaderOutputs, clippingPoints, &clippingPointsCount);
        if (clippingPointsCount < 3)
        {
            return;
        }

        // NDC

        // クリップ処理後は多角形になっていることがあるので最初の２辺で判定
        const Vector2 ndcPositions[3] =
        {
            clippingPoints[0].position.GetXY() / clippingPoints[0].position.w,
            clippingPoints[1].position.GetXY() / clippingPoints[1].position.w,
            clippingPoints[2].position.GetXY() / clippingPoints[2].position.w,
        };

        if (!faceCulling(ndcPositions))
        {
            return;
        }


        // VIEWPORT

        ScreenPoint screenPoints[kTriangleClippingPointMaxNum] = {};
        int screenPointsCount = clippingPointsCount;
        for (int j = 0; j < clippingPointsCount; ++j)
        {
            transformScreenCoordinates(*this, &clippingPoints[j], &screenPoints[j]);
        }


        //
        // Rasterize stage
        //

        for (int j = 0; j < screenPointsCount - 2; ++j)
        {
            // 表裏(もしくは裏表)をそれぞれラスタライズ
            rasterizeTriangle(&screenPoints[0], &screenPoints[j + 1], &screenPoints[j + 2]);
            rasterizeTriangle(&screenPoints[j + 2], &screenPoints[j + 1], &screenPoints[0]);

            // debug code
            //rasterizeLine(&screenPoints[0], &screenPoints[j + 1]);
            //rasterizeLine(&screenPoints[j + 1], &screenPoints[j + 2]);
            //rasterizeLine(&screenPoints[j + 2], &screenPoints[0]);
        }
    }


    void Renderer::drawIndexed()
    {
        Renderer& renderingContext = *this;

        VertexShaderOutput vertexShaderOutputs[3] = {};
        int vertexShaderOutputsCount = 0;

        for (int k = 0; k < _indexBufferIndicesNum; ++k)
        {
            uint16_t index = _indexBufferIndices[k];

            {
                //
                // Input Assembly stage
                //

                VertexShaderInput vertexShaderInput = {};
                vertexShaderInput.uniformBlock = _uniformBlock;

                InputAssembly(index, vertexShaderInput.elements);

                //
                // Vertex Shader stage
                // (transform clipping coordinates)
                //

                assert(vertexShaderOutputsCount < std::size(vertexShaderOutputs));
                VertexShaderOutput* vertexShaderOutput = &(vertexShaderOutputs[vertexShaderOutputsCount]);
                vertexShaderOutputsCount++;

                _vertexShaderMainFunc(&vertexShaderInput, vertexShaderOutput);
            }

            switch (_primitiveTopologyType)
            {
            case PrimitiveTopologyType::LineList:
                if (2 == vertexShaderOutputsCount)
                {
                    outputPrimitiveLine(vertexShaderOutputs);
                    vertexShaderOutputsCount = 0;
                }
                break;
            case PrimitiveTopologyType::TriangleList:
                if (3 == vertexShaderOutputsCount)
                {
                    outputPrimitiveTriangle(vertexShaderOutputs);
                    vertexShaderOutputsCount = 0;
                }
                break;
            }
        }
    }

}

