#pragma once

#include <cstdint>
#include <algorithm>

namespace Lib
{
    // std::lerp : c++20 

    //inline float lerp(float a, float b, float t)
    //{
    //    return a + t * (b - a);
    //}

    // std::clamp : c++20 
    
    //inline int clamp(int v, int low, int high)
    //{
    //    return std::min(std::max(low, v), high);
    //}

    inline float clamp(float v, float low, float high)
    {
        return std::min(std::max(low, v), high);
    }

    inline float NormalizeByte(uint8_t val)
    {
        return val / 255.0f;
    }

    inline uint8_t DenormalizeByte(float val)
    {
        return (uint8_t)(255.0f * clamp(val, 0.0f, 1.0f));
    }

    // Bresenham's line algorithm
    class BresenhamLine
    {

    public:

        int x0;
        int y0;
        int x1;
        int y1;

        int dx;
        int dy;
        int sx;
        int sy;

        int err;

        int x;
        int y;

        void setup(int x0, int y0, int x1, int y1)
        {
            this->x0 = x0;
            this->y0 = y0;
            this->x1 = x1;
            this->y1 = y1;

            if (x0 < x1)
            {
                dx = x1 - x0;
                sx = 1;
            }
            else
            {
                dx = x0 - x1;
                sx = -1;
            }

            if (y0 < y1)
            {
                dy = y1 - y0;
                sy = 1;
            }
            else
            {
                dy = y0 - y1;
                sy = -1;
            }

            err = dx - dy;

            x = x0;
            y = y0;
        }

        bool next()
        {
            if (x == x1 && y == y1)
            {
                return false;
            }
            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err = err - dy;
                x = x + sx;
            }
            if (e2 < dx)
            {
                err = err + dx;
                y = y + sy;
            }
            return true;
        }

    };


    // 以下、未使用


    // ビット 1 の個数
    inline uint32_t popcnt(uint32_t bit)
    {
        bit = (bit & 0x55555555) + (bit >> 1 & 0x55555555);
        bit = (bit & 0x33333333) + (bit >> 2 & 0x33333333);
        bit = (bit & 0x0f0f0f0f) + (bit >> 4 & 0x0f0f0f0f);
        bit = (bit & 0x00ff00ff) + (bit >> 8 & 0x00ff00ff);
        bit = (bit & 0x0000ffff) + (bit >> 16 & 0x0000ffff);
        return bit;
    }

    static int clamp8_fast(int x)
    {
        return ((x & ~(x >> 31)) | ((255 - x) >> 31)) & 255;
    }

    // 最も右にある1のみを残した値
    inline uint32_t rightmost_set_bit(uint32_t bit)
    {
        return bit & (~bit + 1);
    }
}
