#pragma once

#include "../Encoder.hpp"
#include "../x86.hpp"

namespace x86 {
    class ASCII_Instruction : public ::x86::Instruction
    {
    public:
        ASCII_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~ASCII_Instruction() override {}

        void evaluateS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

    private:
        bool useImmediate = false;
        Parser::Immediate immediate;

        bool usedReloc = false;
        StringPool::String relocUsedSection;
        bool relocIsExtern;

        uint8_t value;
        uint64_t max;
        uint16_t sizeInBits;
    };
}
