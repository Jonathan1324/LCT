#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Control_Instruction : public x86::Instruction
    {
    public:
        Simple_Control_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Simple_Control_Instruction() override {}
    };

    class JMP_Instruction : public Instruction
    {
    public:
        JMP_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~JMP_Instruction() override {}

        bool optimizeS() override;

    private:
        uint64_t mnemonicI;
        bool canOptimize = false;
    };

    class RET_Instruction : public Instruction
    {
    public:
        RET_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~RET_Instruction() override {}
    };

    class CALL_Instruction : public Instruction
    {
    public:
        CALL_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~CALL_Instruction() override {}
    };
}
