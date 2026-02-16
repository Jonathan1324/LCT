#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Argument_Interrupt_Instruction : public ::x86::Instruction
    {
    public:
        Argument_Interrupt_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Argument_Interrupt_Instruction() override {}
    };

    class Simple_Interrupt_Instruction : public ::x86::Instruction
    {
    public:
        Simple_Interrupt_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Simple_Interrupt_Instruction() override {}
    };
}
