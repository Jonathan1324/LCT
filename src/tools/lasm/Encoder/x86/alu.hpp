#pragma once

#include "../Encoder.hpp"
#include "x86.hpp"

namespace x86 {
    class Two_Argument_ALU_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Two_Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);

        ~Two_Argument_ALU_Instruction() override {}

        void evaluate() override;

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

        // TODO:
        //   Register <-> Immediate:
        //     16, 32, 64 bit:
        //       currently using 0x81 (full immediate)
        //       optimizable to 0x83 (8-bit immediate, sign-extended)
        //       implement

    private:
        uint8_t opcode;

        Parser::Instruction::Operand mainOperand;
        Parser::Instruction::Operand otherOperand;

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

        bool use16BitPrefix = false;
        
        bool useREX = false;
        bool rexW = false;
        bool rexR = false;
        bool rexX = false;
        bool rexB = false;

        bool useModRM = false;
        ::x86::Mod mod_mod;
        uint8_t mod_reg;
        uint8_t mod_rm;
    };

    class Mul_Div_ALU_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Mul_Div_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Mul_Div_ALU_Instruction() override {}

        void evaluate() override;

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

        // TODO:
        //   Register, Register/Memory, Immediate:
        //     16, 32, 64 bit:
        //       currently using 0x69 (full immediate)
        //       optimizable to 0x68 (8-bit immediate, sign-extended)
        //       implement

    private:
        uint8_t opcode;

        Parser::Instruction::Operand mainOperand;
        
        // Only when Two/ThreeOperands
        Parser::Instruction::Operand secondOperand;
        Parser::Immediate thirdOperand;

        struct {
            uint64_t max;
            uint16_t sizeInBits;

            uint64_t value;
        } threeOperandsSpecific;

        enum class MulDivType {
            Simple,
            TwoOperands,
            ThreeOperands
        } mulDivType;

        bool use16BitPrefix = false;
        
        bool useREX = false;
        bool rexW = false;
        bool rexR = false;
        bool rexX = false;
        bool rexB = false;

        bool useOpcodeEscape = false;

        bool useModRM = false;
        ::x86::Mod mod_mod;
        uint8_t mod_reg;
        uint8_t mod_rm;
    };
}
