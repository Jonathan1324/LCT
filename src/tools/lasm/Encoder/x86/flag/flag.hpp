#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Flag_Instruction : public ::x86::Instruction
    {
    public:
        Simple_Flag_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Simple_Flag_Instruction() override {}
    };
}
