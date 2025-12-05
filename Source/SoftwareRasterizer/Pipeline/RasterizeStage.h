#pragma once

#include "..\Modules\Rasterizer.h"
#include "..\State\WindowSize.h"
#include "..\State\VaryingIndexState.h"
#include "..\State\RasterizerState.h"
#include "..\State\Viewport.h"
#include "..\State\DepthRange.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    struct RasterPrimitive
    {
        PrimitiveType primitiveType;
        VertexDataB vertices[3];
        int vertexNum;
    };

    class RasterizeStage
	{

    public:

        static void validateState(const Viewport* state);

        RasterizeStage();
        ~RasterizeStage();

        void input(const WindowSize* windowSize) { _windowSize = windowSize; }
        void input(const VaryingIndexState* varyingIndexState) { _varyingIndexState = varyingIndexState; }
        void input(const RasterizerState* rasterizerState) { _rasterizerState = rasterizerState; }
        void input(const Viewport* viewport) { _viewport = viewport; }
        void input(const DepthRange* depthRange) { _depthRange = depthRange; }
   
        void output(QuadFragmentData* quadFragment) { _quadFragment = quadFragment; }
        void output(class RenderingContext* renderingContext) { _renderingContext = renderingContext; }

        void prepareRasterize();

        void rasterizePrimitive(RasterPrimitive& rasterPrimitive);

    private:

        void applyPerspectiveDivide(const VertexDataB* vertex, VertexDataC* ndcVertex);

        Vector2 transformNdcToWindowCoord(const VertexDataC* ndcVertex) const;

        float mapDepthRange(float z) const;

        void applyViewportTransform(const VertexDataB* clippedPrimitiveVertices, const VertexDataC* ndcVertex, VertexDataD* rasterizationPoint) const;

        float edgeFunction(const Vector2& a, const Vector2& b, const Vector2& c)
        {
            // TODO：完全なエッジ関数
            // 参考 Juan Pineda 1988 A Parallel Algorithm for Polygon Rasterization. 

            Vector2 ab = b - a;
            Vector2 ac = c - a;
            return ab.cross(ac);
        }

        void rasterizeLine(const VertexDataD* p0, const VertexDataD* p1);
        void rasterizeTriangle(const VertexDataD* rasterizationPoint0, const VertexDataD* rasterizationPoint1, const VertexDataD* rasterizationPopint2);

        void getLineFragment(int x, int y, const VertexDataD* p0, const VertexDataD* p1, FragmentData* fragment);
        void getTriangleFragment(int x, int y, const VertexDataD* p0, const VertexDataD* p1, const VertexDataD* p2, FragmentData* fragment);

    private:

        // input
        const WindowSize* _windowSize = nullptr;
        const VaryingIndexState* _varyingIndexState;
        const RasterizerState* _rasterizerState = nullptr;
        const Viewport* _viewport = nullptr;
        const DepthRange* _depthRange = nullptr;

        // output
        QuadFragmentData* _quadFragment;
        class RenderingContext* _renderingContext = nullptr;

    private:

        int _clipRectMinX = 0;
        int _clipRectMinY = 0;
        int _clipRectMaxX = 0;
        int _clipRectMaxY = 0;

        Rasterizer _rasterizer;

        float _sarea_abc = 0.0f;// singed area 2x

    };
}
