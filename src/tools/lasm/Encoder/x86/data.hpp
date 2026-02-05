#pragma once

#include "../Encoder.hpp"
#include "x86.hpp"

namespace x86 {
    class Mov_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Mov_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);

        ~Mov_Instruction() override {}

        void evaluate() override;

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

    private:
        uint8_t opcode;

        Parser::Instruction::Operand destinationOperand;
        Parser::Instruction::Operand sourceOperand;

        enum class MovType {
            MOV_REG_REG,
            
            MOV_REG_IMM,
        } movType;

        union 
        {
            struct {
                uint64_t max;
                uint16_t sizeInBits;

                uint64_t value;
            } mov_reg_imm;
        } specific;

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
