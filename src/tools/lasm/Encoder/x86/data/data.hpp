#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Mov_Instruction : public ::x86::Instruction
    {
    public:
        Mov_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);

        ~Mov_Instruction() override {}

        void evaluateS() override;

        bool optimizeS() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;

        // TODO:
        //   Optimize

    private:
        Parser::Instruction::Operand destinationOperand;
        Parser::Instruction::Operand sourceOperand;

        enum class MovType {
            MOV_REG_REG,
            
            MOV_REG_IMM,
        } movType;

        bool can_optimize = false;
        bool needs_rex = false;
        bool is_8_bit = false;
        Parser::Instruction::Register optimize_reg;

        union 
        {
            struct {
                uint64_t max;
                uint16_t sizeInBits;

                uint64_t value;
            } mov_reg_imm;
        } specific;

        bool usedReloc = false;
        std::string relocUsedSection;
        bool relocIsExtern;
    };
}
