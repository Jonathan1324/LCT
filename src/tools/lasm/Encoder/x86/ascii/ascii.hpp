#pragma once

#include "../Encoder.hpp"
#include "../x86.hpp"

namespace x86 {
    class ASCII_Instruction : public ::x86::Instruction
    {
    public:
        ASCII_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~ASCII_Instruction() override {}
    };
}
