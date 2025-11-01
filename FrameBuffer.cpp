
// ウィンドウ相対座標（ビューポート変換後）
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//
//
// Windows GUI の座標系
//
//   (0, 0)
//         +---- +x
//         |
//         |
//       +y                
//
//
// DIBのメモリを直接参照したとき座標系
//
//       +y                
//         |
//         |
//         +---- +x
//   (0, 0)
//

#include "FrameBuffer.h"
#include "Algorithm.h"
#include <cassert>
#include <algorithm>// fill

namespace SoftwareRasterizer
{
    void FrameBuffer::setFrameSize(int width, int height)
    {
        _freameWidth = width;
        _freameHeight = height;
    }

    void FrameBuffer::setColorBuffer(void* addr, size_t widthBytes)
    {
        _colorBuffer = addr;
        _colorBufferWidthBytes = widthBytes;
    }

    void FrameBuffer::setDepthBuffer(void* addr, size_t widthBytes)
    {
        _depthBuffer = addr;
        _depthBufferWidthBytes = widthBytes;
    }

    void FrameBuffer::setClearColor(float r, float g, float b, float a)
    {
        _clearColor = (denormalizeByte(a) << 24) | (denormalizeByte(r) << 16) | (denormalizeByte(g) << 8) | denormalizeByte(b);
    }

    void FrameBuffer::setClearDepth(float depth)
    {
        _clearDepth = depth;
    }

    void FrameBuffer::clearBuffer()
	{
        assert(0 <= _freameWidth);
        assert(0 <= _freameHeight);
        assert(nullptr != _colorBuffer);
        assert(_freameWidth <= _colorBufferWidthBytes);
        assert(nullptr != _depthBuffer);
        assert(_freameWidth <= _depthBufferWidthBytes);

        uint32_t* firstColor = (uint32_t*)_colorBuffer;
        uint32_t* lastColor = ((uint32_t*)_colorBuffer) + (_freameHeight * _freameWidth);
        std::fill(firstColor, lastColor, _clearColor);

        float* firstDepth = (float*)_depthBuffer;
        float* lastDepth = ((float*)_depthBuffer) + (_freameHeight * _freameWidth);
        std::fill(firstDepth, lastDepth, _clearDepth);
    }

    float FrameBuffer::readDepth(int x, int y) const
    {
        // x, y はウィンドウ相対座標

        if (0 <= x && x < _freameWidth || 0 <= y || y < _freameHeight)
        {
            size_t depthOffset = (_depthBufferWidthBytes * y) + (sizeof(float) * x);
            const float* depthSrc = (float*)(((uintptr_t)_depthBuffer) + depthOffset);
            return *depthSrc;
        }
        else
        {
            return _clearDepth;
        }
    }
    
    void FrameBuffer::writePixel(int x, int y, const Vector4& color, float depth)
    {
        // x, y はウィンドウ相対座標
        
        if (0 <= x && x < _freameWidth || 0 <= y || y < _freameHeight)
        {
            // DIBも左下が(0,0)なので上下反転は不要
            size_t colorOffset = (_colorBufferWidthBytes * y) + (sizeof(uint32_t) * x);
            uint32_t* colorDst = (uint32_t*)(((uintptr_t)_colorBuffer) + colorOffset);
            uint32_t r = denormalizeByte(color.x);
            uint32_t g = denormalizeByte(color.y);
            uint32_t b = denormalizeByte(color.z);
            *colorDst = (r << 16) | (g << 8) | (b);

            size_t depthOffset = (_depthBufferWidthBytes * y) + (sizeof(float) * x);
            float* depthDst = (float*)(((uintptr_t)_depthBuffer) + (_depthBufferWidthBytes * y) + (sizeof(float) * x));
            *depthDst = depth;
        }
    }
}
