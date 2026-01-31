#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include <tuple>

#include "control.hpp"
#include "interrupt.hpp"
#include "flag.hpp"
#include "stack.hpp"
#include "data.hpp"

namespace x86
{
    class Encoder : public ::Encoder::Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        ~Encoder() = default;

    protected:
        ::Encoder::Encoder::Instruction* GetInstruction(const Parser::Instruction::Instruction& instruction) override;

        bool OptimizeOffsets(std::vector<Parser::Section>& parsedSections) override;

        std::vector<uint8_t> EncodeInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved = false, bool optimize = false) override;
        uint64_t GetSize(Parser::Instruction::Instruction& instruction) override;
        std::vector<uint8_t> EncodePadding(size_t length) override;

    private:
        bool instrUse16BitPrefix = false;

        static constexpr const uint8_t opcodeEscape = 0x0F;

        inline uint8_t getRex(bool W, bool R, bool X, bool B)
        {
            uint8_t rex = 0b01000000;
            if (W) rex |= 0b00001000;
            if (R) rex |= 0b00000100;
            if (X) rex |= 0b00000010;
            if (B) rex |= 0b00000001;
            return rex;
        }

        // first bool = use rex
        // second bool = set rex
        std::tuple<uint8_t, bool, bool> getReg(uint64_t reg);
        uint8_t getRegSize(uint64_t reg, BitMode mode);

        std::vector<uint8_t> EncodeControlInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize);
        std::vector<uint8_t> EncodeInterruptInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize);
        std::vector<uint8_t> EncodeFlagInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize);
        std::vector<uint8_t> EncodeStackInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize);
        std::vector<uint8_t> EncodeDataInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize);
    };
}
