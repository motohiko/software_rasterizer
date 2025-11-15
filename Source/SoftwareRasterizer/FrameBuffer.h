#pragma once

#include "Types.h"
#include "..\Lib\Vector.h"
#include <cstdint>

namespace SoftwareRasterizer
{
    using Lib::Vector4;

    class FrameBuffer
    {

    public:

        void setFrameSize(int width, int height);
        int getFrameWidth() const { return _freameWidth; };
        int getFrameHeight() const { return _freameHeight; };

        void setColorBuffer(void* addr, size_t widthBytes);
        void setDepthBuffer(void* addr, size_t widthBytes);

        void setClearColor(float r, float g, float b, float a);// glClearColor
        void setClearDepth(float depth);// glClearDepth

        void clearBuffer();// glClear()

        float readDepth(int x, int y) const;
        void writePixel(int x, int y, const Vector4& color, float depth);

    private:

        int _freameWidth = 0;
        int _freameHeight = 0;

        void* _colorBuffer = nullptr;// uint32_t[], BGRA
        size_t _colorBufferWidthBytes = 0;

        void* _depthBuffer = nullptr;// float[]
        size_t _depthBufferWidthBytes = 0;

        uint32_t _clearColor = 0;
        float _clearDepth = 1.0f; 

    };
}
