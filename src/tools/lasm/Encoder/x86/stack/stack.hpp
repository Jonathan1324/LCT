#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Normal_Stack_Instruction : public Instruction
    {
    public:
        Normal_Stack_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr);
        ~Normal_Stack_Instruction() override {}

    protected:
        bool optimizeS() override;

    private:
        bool canOptimize = false;
    };

    class Simple_Stack_Instruction : public Instruction
    {
    public:
        Simple_Stack_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr);
        ~Simple_Stack_Instruction() override {}
    };
}
