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

        void execute(int x, int y, const Vector4& color, float depth);

    private:

        float normalizeDepth(float depth) const;
        bool depthTest(float depth, float storedDepth) const;

        void storeTexelColor(int x, int y, const Vector4& color);

        float fetchTexelDepth(int x, int y) const;
        void storeTexelDepth(int x, int y, float depth);

    private:

        // input
        const DepthState* _depthState = nullptr;
        const DepthRange* _depthRange = nullptr;

        // output
        RenderTarget* _renderTarget = nullptr;
        
    };
}
