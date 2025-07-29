#pragma once

#include "Vector.h"
#include <cstdint>

namespace SoftwareRenderer
{
    class FrameBuffer
    {

    private:

        struct ColorFrameBuffer
        {
            void* addr;// byte[], BGRA
            size_t widthBytes;
        };

        struct DepthFrameBuffer
        {
            void* addr;// float[]
            size_t widthBytes;
        };

        int _freameWidth = 0;
        int _freameHeight = 0;

        ColorFrameBuffer colorFrameBuffer {};
        DepthFrameBuffer depthFrameBuffer {};

    public:

        void setFrameSize(int width, int height);
        int getFrameWidth() const { return _freameWidth; };
        int getFrameHeight() const { return _freameHeight; };

        void setRenderColorBuffer(void* addr, size_t widthBytes);
        void setRenderDepthBuffer(void* addr, size_t widthBytes);

        void clearRenderBuffer();

        float readDepth(int x, int y) const;
        void writeColorAndDepth(int x, int y, const Vector4& color, float depth);
    };
}
