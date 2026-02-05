#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Argument_Interrupt_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Argument_Interrupt_Instruction() override {}

        void evaluate() override;

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

    private:
        uint8_t opcode;

        Parser::Immediate argument;

        uint8_t argument_value;
    };

    class Simple_Interrupt_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Interrupt_Instruction() override {}

        void evaluate() override {}

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

    private:
        uint8_t opcode;

        bool use16BitPrefix = false;

        bool useREX = false;
        bool rexW = false;
        bool rexR = false;
        bool rexX = false;
        bool rexB = false;

        bool useOpcodeEscape = false;
    };
}
