#pragma once

#include <architecture/architecture.hpp>
#include <cstdint>
#include <tuple>
#include <x86/Registers.hpp>
#include <Exception.hpp>

namespace x86 {
    constexpr uint8_t prefix16Bit = 0x66;
    constexpr uint8_t addressPrefix16Bit = 0x67;

    constexpr uint8_t opcodeEscapeFirst = 0x0F;
    constexpr uint8_t opcodeEscape38 = 0x38;
    constexpr uint8_t opcodeEscape3A = 0x3A;

    constexpr uint8_t repPrefix = 0xF3;

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
    constexpr uint8_t modRMSIB = 0b100;
    constexpr uint8_t modRMDisp = 0b101;
    inline uint8_t getModRM(Mod mod, uint8_t reg, uint8_t rm)
    {
        uint8_t modrm = ((uint8_t(mod)) << 6) | (reg << 3) | rm;
        return modrm;
    }

    enum class Scale : uint8_t {
        x1 = 0,
        x2 = 1,
        x4 = 2,
        x8 = 3
    };
    constexpr uint8_t SIB_NoIndex = 0b100;
    inline uint8_t getSIB(Scale scale, uint8_t index, uint8_t base)
    {
        uint8_t sib = ((uint8_t(scale)) << 6) | (index << 3) | base;
        return sib;
    }

    /*
        tuple:
            uint8_t: register index
            bool: use REX to address
            bool: set REX byte
    */
    std::tuple<uint8_t, bool, bool> getReg(uint64_t reg);

    uint8_t getRegSize(uint64_t reg, BitMode mode);

    bool isGPR(uint64_t reg);
    bool isSegment(uint64_t reg);
}
