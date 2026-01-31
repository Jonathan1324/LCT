#pragma once

#include <architecture/architecture.hpp>
#include <cstdint>
#include <tuple>
#include <x86/Registers.hpp>
#include <Exception.hpp>

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

    enum class Mod : uint8_t {
        INDIRECT        = 0b00,
        INDIRECT_DISP8  = 0b01,
        INDIRECT_DISP32 = 0b10,
        REGISTER        = 0b11
    };
    inline uint8_t getModRM(Mod mod, uint8_t reg, uint8_t rm)
    {
        uint8_t modrm = ((uint8_t(mod)) << 6) | (reg << 3) | rm;
        return modrm;
    }

    /*
        tuple:
            uint8_t: register index
            bool: use REX to address
            bool: set REX byte
    */
    std::tuple<uint8_t, bool, bool> getReg(uint64_t reg);

    uint8_t getRegSize(uint64_t reg, BitMode mode);
}
