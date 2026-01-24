#pragma once

#include <cstdint>

namespace x86 {
    constexpr uint8_t prefix16Bit = 0x66;
    constexpr uint8_t opcodeEscape = 0x0F;

    inline uint8_t getRex(bool W, bool R, bool X, bool B)
    {
        uint8_t rex = 0b01000000;
        if (W) rex |= 0b00001000;
        if (R) rex |= 0b00000100;
        if (X) rex |= 0b00000010;
        if (B) rex |= 0b00000001;
        return rex;
    }
}
