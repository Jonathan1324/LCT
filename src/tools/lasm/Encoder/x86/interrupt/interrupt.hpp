#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Argument_Interrupt_Instruction : public ::x86::Instruction
    {
    public:
        Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Argument_Interrupt_Instruction() override {}

        void evaluateS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

    private:
        bool usedReloc = false;
        StringPool::String relocUsedSection;
        bool relocIsExtern;

        Parser::Immediate argument;

        uint8_t argument_value;
    };

    class Simple_Interrupt_Instruction : public ::x86::Instruction
    {
    public:
        Simple_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Interrupt_Instruction() override {}
    };
}
