#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Mov_Instruction : public ::x86::Instruction
    {
    public:
        Mov_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Mov_Instruction() override {}
        
        bool optimizeS() override;

    private:
        bool can_optimize = false;
        bool needs_rex = false;
        bool is_8_bit = false;
        Parser::Instruction::Register optimize_reg;
    };
}
