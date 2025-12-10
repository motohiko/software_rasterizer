#pragma once

#include "..\State\DepthState.h"
#include "..\State\DepthRange.h"
#include "..\State\RenderTarget.h"
#include "..\Core\Types.h"

namespace SoftwareRasterizer
{
    class OutputMergerStage
    {

    public:

        OutputMergerStage();

        void input(const DepthState* depthState) { _depthState = depthState; }
        void input(const DepthRange* depthRange) { _depthRange = depthRange; }

        void output(RenderTarget* renderTarget) { _renderTarget = renderTarget; }

        void execute(const IntVector2& texelCoord, const PixelData* pixel);

    private:

        float normalizeDepth(float depth) const;
        bool depthTest(float depth, float storedDepth) const;

        void storeTexelColor(const IntVector2& texelCoord, const Vector4& color);

        float fetchTexelDepth(const IntVector2& texelCoord) const;
        void storeTexelDepth(const IntVector2& texelCoord, float depth);

    private:

        // input
        const DepthState* _depthState = nullptr;
        const DepthRange* _depthRange = nullptr;

        // output
        RenderTarget* _renderTarget = nullptr;
        
    };
}
