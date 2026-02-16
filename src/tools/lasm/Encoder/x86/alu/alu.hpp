#pragma once

#include "../Encoder.hpp"
#include "../x86.hpp"

namespace x86 {
    class Two_Argument_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Two_Argument_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Two_Argument_ALU_Instruction() override {}

        bool optimizeS() override;

    private:
        bool canOptimize = false;
        bool accumulatorReg = false;
        Parser::Instruction::Operand mainOperand;
    };

    class Mul_Div_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Mul_Div_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Mul_Div_ALU_Instruction() override {}

        bool optimizeS() override;

    private:
        bool canOptimize = false;
    };

    class Shift_Rotate_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Shift_Rotate_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Shift_Rotate_ALU_Instruction() override {}

    protected:
        bool optimizeS() override;

    private:
        bool canOptimize = false;
        bool is8Bit = false;
    };

    class Argument_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Argument_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~Argument_ALU_Instruction() override {}
    };

    class ADX_ALU_Instruction : public Instruction
    {
    public:
        ADX_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr);
        ~ADX_ALU_Instruction() override {}
    };
}
