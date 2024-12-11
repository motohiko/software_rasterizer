#pragma once

#include <cstdint>
#include <algorithm>

namespace MyApp
{
    // std::lerp : c++20 
    inline float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    // std::clamp : c++20 
    inline int clamp(int v, int low, int high)
    {
        return std::min(std::max(low, v), high);
    }

    inline float clamp(float v, float low, float high)
    {
        return std::min(std::max(low, v), high);
    }


    // 以下、未使用

    // １ビットの数
    inline uint32_t popcnt(uint32_t bit)
    {
        bit = (bit & 0x55555555) + (bit >> 1 & 0x55555555);
        bit = (bit & 0x33333333) + (bit >> 2 & 0x33333333);
        bit = (bit & 0x0f0f0f0f) + (bit >> 4 & 0x0f0f0f0f);
        bit = (bit & 0x00ff00ff) + (bit >> 8 & 0x00ff00ff);
        bit = (bit & 0x0000ffff) + (bit >> 16 & 0x0000ffff);
        return bit;
    }

    // 最も右にある1のみを残した値
    inline uint32_t rightmost_set_bit(uint32_t bit)
    {
        return bit & (~bit + 1);
    }
}
