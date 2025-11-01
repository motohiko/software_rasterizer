#include "RenderingContext.h"
#include "Algorithm.h"
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <cassert>

namespace SoftwareRasterizer
{
    RenderingContext::RenderingContext() : 
        _rasterizeStage(&_rasterizeStageState),
        _fragmentShaderStage(&_fragmentShaderStageState)
    {
    }

    void RenderingContext::setFrameSize(int width, int height)
    {
        _frameBuffer.setFrameSize(width, height);
    }

    int RenderingContext::getFrameWidth() const
    {
        return _frameBuffer.getFrameWidth();
    };

    int RenderingContext::getFrameHeight() const
    {
        return _frameBuffer.getFrameHeight();
    };

    void RenderingContext::setFrameColorBuffer(void* addr, size_t widthBytes)
    {
        _frameBuffer.setColorBuffer(addr, widthBytes);
    }

    void RenderingContext::setFrameDepthBuffer(void* addr, size_t widthBytes)
    {
        _frameBuffer.setDepthBuffer(addr, widthBytes);
    }

    void RenderingContext::setClearColor(float r, float g, float b, float a)
    {
        _frameBuffer.setClearColor(r, g, b, a);
    }

    void RenderingContext::setClearDepth(float depth)
    {
        _frameBuffer.setClearDepth(depth);
    }

    void RenderingContext::clearFrameBuffer()
    {
        _frameBuffer.clearBuffer();
    }

    void RenderingContext::enableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        _inputAssemblyStageState.vertexAttributeEnableBits |= (1u << index);
    }

    void RenderingContext::disableVertexAttribute(int index)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        _inputAssemblyStageState.vertexAttributeEnableBits &= ~(1u << index);
    }

    void RenderingContext::setVertexBuffer(int index, const void* buffer)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        VertexAttributeLayout* attribute = &(_inputAssemblyStageState.vertexAttributeLayouts[index]);
        attribute->buffer = buffer;
    }

    void RenderingContext::setVertexAttribute(int index, Semantics semantics, int size, ComponentType type, size_t stride)
    {
        assert(0 <= index && index < std::size(_inputAssemblyStageState.vertexAttributeLayouts));
        VertexAttributeLayout* attribute = &(_inputAssemblyStageState.vertexAttributeLayouts[index]);
        attribute->semantics = semantics;
        attribute->size = size;
        attribute->type = type;
        attribute->normalized = false;
        attribute->stride = stride;
    }

    void RenderingContext::setIndexBuffer(const uint16_t* indices, int indexNum)
    {
        _inputAssemblyStageState.indexBuffer.indices = indices;
        _inputAssemblyStageState.indexBuffer.indexNum = indexNum;
    }

    void RenderingContext::setPrimitiveTopologyType(PrimitiveTopologyType primitiveTopologyType)
    {
        _inputAssemblyStageState.primitiveTopologyType = primitiveTopologyType;
    }

    void RenderingContext::setUniformBlock(const void* uniformBlock)
    {
        _vertexShaderStageState.uniformBlock = uniformBlock;
        _fragmentShaderStageState.uniformBlock = uniformBlock;
    }

    void RenderingContext::setVertexShaderProgram(VertexShaderFuncPtr vertexShaderMain)
    {
        _vertexShaderStageState.vertexShaderMain = vertexShaderMain;
    }

    void RenderingContext::setViewport(int x, int y, int width, int height)
    {
        _rasterizeStageState.viewportX = x;
        _rasterizeStageState.viewportY = y;
        _rasterizeStageState.viewportWidth = width;
        _rasterizeStageState.viewportHeight = height;
    }

    int RenderingContext::getViewportWidth() const
    {
        return _rasterizeStageState.viewportWidth;
    }

    int RenderingContext::getViewportHeight() const
    {
        return _rasterizeStageState.viewportHeight;
    }

    void RenderingContext::setDepthRange(float nearVal, float farVal)
    {
        _rasterizeStageState.depthRangeNearVal = nearVal;
        _rasterizeStageState.depthRangeFarVal = farVal;
    }

    void RenderingContext::setFragmentShaderProgram(FragmentShaderFuncPtr fragmentShaderMain)
    {
        _fragmentShaderStageState.fragmentShaderMain = fragmentShaderMain;
    }

    void RenderingContext::drawIndexed()
    {
        InputAssemblyStage inputAssemblyStage(&_inputAssemblyStageState);
        VertexShaderStage vertexShaderStage(&_vertexShaderStageState);
        ClipStage clipStage;

        inputAssemblyStage.validateState();
        vertexShaderStage.validateState();
        _rasterizeStage.validateState();
        _fragmentShaderStage.validateState();

        inputAssemblyStage.prepareReadPrimitive();
        inputAssemblyStage.prepareReadVertex();

        InputAssemblyStage::Primitive primitive;
        while (inputAssemblyStage.readPrimitive(&primitive))
        {
            ShadedVertex shadedVertices[3];

            for (int i = 0; i < primitive.vertexNum; i++)
            {
                uint16_t vertexIndex = primitive.vertexIndices[i];

                AttributeVertex attributeVertex;
                inputAssemblyStage.readAttributeVertex(vertexIndex, &attributeVertex);

                // 頂点座標をクリッピング空間座標に変換して、必要ならライティング用の情報を設定する
                vertexShaderStage.executeShader(&attributeVertex, &(shadedVertices[i]));
            }

            // プリミティブをクリップ
            ShadedVertex clippedVertices[kTriangleClippingPointMaxNum];
            int clippedVertiexNum = 0;
            clipStage.setPrimitiveType(primitive.primitiveType);
            clipStage.clipPrimitive(shadedVertices, primitive.vertexNum, clippedVertices, &clippedVertiexNum);

            // クリップ結果が多角形ならプリミティブを分割する
            PrimitiveAssembly primitiveAssembly;
            primitiveAssembly.setPrimitiveType(primitive.primitiveType);
            primitiveAssembly.setClipedVertices(clippedVertices, clippedVertiexNum);
            primitiveAssembly.prepareDividPrimitive();
            AssembledPrimitive dividedPrimitive;
            while (primitiveAssembly.readPrimitive(&dividedPrimitive))
            {
                // （分割された）各プリミティブをラスタライズ

                struct RasterPrimitive
                {
                    PrimitiveType primitiveType;
                    ShadedVertex vertices[3];
                    int vertexNum;
                };

                RasterPrimitive rasterPrimitive;
                rasterPrimitive.primitiveType = dividedPrimitive.primitiveType;
                for (int i = 0; i < dividedPrimitive.vertexNum; i++)
                {
                    uint16_t vertexIndex = dividedPrimitive.vertexIndices[i];
                    rasterPrimitive.vertices[i] = clippedVertices[vertexIndex];
                }
                rasterPrimitive.vertexNum = dividedPrimitive.vertexNum;

                // 各頂点を正規化デバイス座標へ変換（W除算）
                NdcVertex ndcVertices[3];
                for (int i = 0; i < rasterPrimitive.vertexNum; i++)
                {
                    _rasterizeStage.transformToNdcVertex(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]));
                }

                // フェイスカリング
                if (PrimitiveType::kTriangle == primitive.primitiveType)
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
                int rasterVertexNum = rasterPrimitive.vertexNum;
                for (int i = 0; i < rasterVertexNum; i++)
                {
                    _rasterizeStage.transformRasterVertex(&(rasterPrimitive.vertices[i]), &(ndcVertices[i]), &rasterVertices[i]);
                }

                switch (rasterVertexNum)
                {
                case 2:
                    rasterizeLine(&rasterVertices[0], &rasterVertices[1]);
                    break;
                case 3:
                    bool wireframe = false;
                    if (!wireframe)
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

    void RenderingContext::rasterizeLine(const RasterVertex* p0, const RasterVertex* p1)
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

            Vector4 color;
            _fragmentShaderStage.executeShader(&fragment, &color);

            if (fragment.helperInvocation)
            {
                continue;
            }

            bool passed = depthTest(x, y, fragment.depth);
            if (!passed)
            {
                continue;
            }

            _frameBuffer.writePixel(x, y, color, fragment.depth);

        } while (bresenhamLine.next());
    }

    void RenderingContext::rasterizeTriangle(const RasterVertex* p0, const RasterVertex* p1, const RasterVertex* p2)
    {
        // 全ての頂点が同一座標（縮退ポリゴン）はラスタライズ出来ない
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

                Vector4 color;
                _fragmentShaderStage.executeShader(&fragment, &color);

                if (fragment.helperInvocation)
                {
                    continue;
                }

                bool passed = depthTest(x, y, fragment.depth);
                if (!passed)
                {
                    continue;
                }

                _frameBuffer.writePixel(x, y, color, fragment.depth);
            }
        }
    }
}
