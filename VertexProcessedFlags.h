#pragma once

#include <cstdint>
#include <cstring>

namespace MyApp
{
    // 未使用

    class VertexProcessedFlags
    {
    private:
        uint64_t bits[0x10000 / 64];
        int maxIndex;

    public:
        VertexProcessedFlags()
        {
            std::memset(bits, 0, 0x10000 / 8);
            maxIndex = -1;
        }

        void SetFlag(uint16_t index)
        {
            bits[index / 64] |= ((uint64_t)1) << (index % 64);
            maxIndex = std::max(maxIndex, (int)index);
        }

        bool GetFlag(uint16_t index) const
        {
            return 0 != (bits[index / 64] & ((uint64_t)1) << (index % 64));
        }

        void Clear()
        {
            std::memset(bits, 0, 1 + (maxIndex / 8));
            maxIndex = -1;
        }
    };
}
