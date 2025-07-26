#include "Renderer.h"
#include "Algorithm.h"
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>

namespace MyApp
{
    void Renderer::drawIndexed()
    {
        _inputAssemblyStage.resetReadCount();

        for (;;)
        {
            // プリミティブを読み取る
            AssembledPrimitive assembledPrimitive;
            if (!_inputAssemblyStage.readPrimitive(&assembledPrimitive))
            {
                break;
            }

            // 各頂点を座標をクリッピング空間座標に変換して、必要ならライティング用の情報を設定する
            Primitive primitive;
            for (int i = 0; i < assembledPrimitive.vertexNum; i++)
            {
                _vertexShaderStage.executeShader(&(assembledPrimitive.vertices[i]), &(primitive.vertices[i]));
            }
            primitive.vertexNum = assembledPrimitive.vertexNum;

            // 視錐台でプリミティブをクリップして、クリップした結果が多角形なら複数の３角形に分割する
            Vertex clippedPrimitiveVertices[kTriangleClippingPointMaxNum];
            int clippedPrimitiveVertiexCount = 0;
            Primitive clippedPrimitives[(kTriangleClippingPointMaxNum - 2)];
            int clippedPrimitiveCount = 0;
            switch (primitive.vertexNum)
            {
            case 2:
                _rasterizeStage.clipPrimitiveLine(primitive.vertices, primitive.vertexNum, clippedPrimitiveVertices, &clippedPrimitiveVertiexCount);
                if (2 == clippedPrimitiveVertiexCount)
                {
                    clippedPrimitives[0].vertices[0] = clippedPrimitiveVertices[0];
                    clippedPrimitives[0].vertices[1] = clippedPrimitiveVertices[1];
                    clippedPrimitives[0].vertexNum = 2;
                    clippedPrimitiveCount++;
                }
                break;
            case 3:
                _rasterizeStage.clipPrimitiveTriangle(primitive.vertices, primitive.vertexNum, clippedPrimitiveVertices, &clippedPrimitiveVertiexCount);
                for (int i = 0; i < (clippedPrimitiveVertiexCount - 2); i++)
                {
                    clippedPrimitives[i].vertices[0] = clippedPrimitiveVertices[0];
                    clippedPrimitives[i].vertices[1] = clippedPrimitiveVertices[i + 1];
                    clippedPrimitives[i].vertices[2] = clippedPrimitiveVertices[i + 2];
                    clippedPrimitives[i].vertexNum = 3;
                    clippedPrimitiveCount++;
                }
                break;
            }

            // （分割された）各プリミティブをラスタライズ
            for (int i = 0; i < clippedPrimitiveCount; i++)
            {
                const Primitive* clippedPrimitive = &(clippedPrimitives[i]);

                // 各頂点を正規化デバイス座標へ変換（W除算）
                NdcVertex ndcVertices[3];
                for (int j = 0; j < clippedPrimitive->vertexNum; j++)
                {
                    _rasterizeStage.transformToNdcVertex(&(clippedPrimitive->vertices[j]), &(ndcVertices[j]));
                }

                // フェイスカリング
                if (3 == clippedPrimitive->vertexNum)
                {
                    Vector2 p0 = ndcVertices[0].ndcPosition.getXY();
                    Vector2 p1 = ndcVertices[1].ndcPosition.getXY();
                    Vector2 p2 = ndcVertices[2].ndcPosition.getXY();

                    // glFrontFace(GL_CCW) // OpenGL default
                    // glEnable(GL_CULL_FACE)
                    float n = (p1 - p0).cross(p2 - p0);// CCW
                    if (n <= 0.0f)// GL_CULL_FACE
                    {
                        continue;
                    }
                }

                // ビューポート変換とデプス値へのマッピング
                RasterVertex rasterVertices[3];
                int rasterVertexNum;
                for (int j = 0; j < clippedPrimitive->vertexNum; j++)
                {
                    _rasterizeStage.transformRasterVertex(&(clippedPrimitive->vertices[j]), &(ndcVertices[j]), &rasterVertices[j]);
                }
                rasterVertexNum = clippedPrimitive->vertexNum;

                // ラスタライズ
                switch (rasterVertexNum)
                {
                case 2:
                    rasterizeLine(&rasterVertices[0], &rasterVertices[1]);
                    break;
                case 3:
                    if (true)
                    {
                        rasterizeTriangle(&rasterVertices[0], &rasterVertices[1], &rasterVertices[2]);
                    }
                    else
                    {
                        rasterizeLine(&rasterVertices[0], &rasterVertices[1]);
                        rasterizeLine(&rasterVertices[1], &rasterVertices[2]);
                        rasterizeLine(&rasterVertices[2], &rasterVertices[0]);
                    }
                    break;
                }
            }
        }
    }

    void Renderer::rasterizeLine(const RasterVertex* p0, const RasterVertex* p1)
    {
        BresenhamLine bresenhamLine;

        int x0 = (int)std::floor(p0->wrcPosition.x);// 小数点以下切り捨て
        int y0 = (int)std::floor(p0->wrcPosition.y);
        int x1 = (int)std::floor(p1->wrcPosition.x);
        int y1 = (int)std::floor(p1->wrcPosition.y);
        bresenhamLine.setup(x0, y0, x1, y1);

        do
        {
            int x = bresenhamLine.x;
            int y = bresenhamLine.y;

            int width = _frameBuffer.getFrameWidth();
            int height = _frameBuffer.getFrameHeight();
            if (x < 0 || width <= x || y < 0 || height <= y)
            {
                continue;
            }

            Fragment fragment;
            _rasterizeStage.getLineFragment(x, y, p0, p1, &fragment);

            Vector4 outputColor;
            _fragmentShaderStage.executeShader(&fragment, &outputColor);

            if (fragment.helperInvocation)
            {
                continue;
            }

            bool passed = depthTest(x, y, fragment.depth);
            if (!passed)
            {
                continue;
            }

            _frameBuffer.writeColorAndDepth(x, y, outputColor, fragment.depth);

        } while (bresenhamLine.next());
    }

    void Renderer::rasterizeTriangle(const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2)
    {
        // 全頂点が同一座標（縮退ポリゴン）はラスタライズ出来ない
        float area2 = RasterizeStage::edgeFunction(p0->wrcPosition, p1->wrcPosition, p2->wrcPosition);
        if (0.0f == area2)
        {
            return;
        }

        // 処理が重すぎるのでラスタライズの範囲を絞り込む
        struct BoundingBox2d
        {
            float minX;
            float minY;
            float maxX;
            float maxY;
            void init()
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
        boundingBox.init();
        boundingBox.addPoint(p0->wrcPosition);
        boundingBox.addPoint(p1->wrcPosition);
        boundingBox.addPoint(p2->wrcPosition);

        int minX = (int)std::floor(boundingBox.minX);// 切り捨て
        int minY = (int)std::floor(boundingBox.minY);
        int maxX = (int)std::ceil(boundingBox.maxX);// 切り上げ
        int maxY = (int)std::ceil(boundingBox.maxY);
        int width = _frameBuffer.getFrameWidth();
        int height = _frameBuffer.getFrameHeight();
        minX = clamp(minX, 0, width - 1);
        minY = clamp(minY, 0, height - 1);
        maxX = clamp(maxX, 0, width - 1);
        maxY = clamp(maxY, 0, height - 1);

        for (int y = maxY; minY <= y; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                Fragment fragment;
                _rasterizeStage.getTriangleFragment(x, y, p0, p1, p2, &fragment);

                 Vector4 outputColor;
                _fragmentShaderStage.executeShader(&fragment, &outputColor);

                if (fragment.helperInvocation)
                {
                    continue;
                }

                bool passed = depthTest(x, y, fragment.depth);
                if (!passed)
                {
                    continue;
                }

                _frameBuffer.writeColorAndDepth(x, y, outputColor, fragment.depth);
            }
        }
    }
}
