#pragma once

#include "../Encoder.hpp"
#include "../x86.hpp"

namespace x86 {
    class Two_Argument_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Two_Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);

        ~Two_Argument_ALU_Instruction() override {}

        void evaluateS() override;

        bool optimizeS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

    private:
        Parser::Instruction::Operand mainOperand;
        Parser::Instruction::Operand otherOperand;

        BitMode bitmode;
        bool canOptimize = false;
        bool accumulatorReg = false;

        enum class AluType {
            ALU_REG_REG,

            ALU_REG_IMM,
        } aluType;

        union
        {
            struct {
                uint64_t max;
                uint16_t sizeInBits;

                uint64_t value;
            } alu_reg_imm;
        } specific;

        bool usedReloc = false;
        std::string evalUsedSection;
        bool evalIsExtern;
    };

    class Mul_Div_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Mul_Div_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Mul_Div_ALU_Instruction() override {}

        void evaluateS() override;

        bool optimizeS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

    private:
        Parser::Instruction::Operand mainOperand;
        
        // Only when Two/ThreeOperands
        Parser::Instruction::Operand secondOperand;
        Parser::Immediate thirdOperand;

        bool canOptimize = false;

        struct {
            uint64_t max;
            uint16_t sizeInBits;

            uint64_t value;
        } threeOperandsSpecific;

        bool usedReloc = false;
        std::string evalUsedSection;
        bool evalIsExtern;

        enum class MulDivType {
            Simple,
            TwoOperands,
            ThreeOperands
        } mulDivType;
    };

    class Shift_Rotate_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Shift_Rotate_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Shift_Rotate_ALU_Instruction() override {}

        void evaluateS() override;

        bool optimizeS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

        // TODO:
        //   Register, Register/Memory, Immediate:
        //     16, 32, 64 bit:
        //       currently using 0xD2/D3 (with immediate)
        //       should use shift by 1 opcode when count = 1

    private:
        bool usedReloc = false;
        std::string relocUsedSection;
        bool relocIsExtern;

        bool usesImmediate = false;
        uint8_t count;

        Parser::Instruction::Operand mainOperand;
        Parser::Instruction::Operand countOperand;
    };

    class Argument_ALU_Instruction : public ::x86::Instruction
    {
    public:
        Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Argument_ALU_Instruction() override {}

    private:
        bool usedReloc = false;
        std::string evalUsedSection;
        bool evalIsExtern;
    };
}
