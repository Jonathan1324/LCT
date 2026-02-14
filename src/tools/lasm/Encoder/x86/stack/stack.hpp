#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Normal_Stack_Instruction : public Instruction
    {
    public:
        Normal_Stack_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~Normal_Stack_Instruction() override {}

    protected:
        void evaluateS() override;
        bool optimizeS() override;
        void encodeS(std::vector<uint8_t>& buffer) override;
        uint64_t sizeS() override;

    private:
        bool usedReloc = false;
        StringPool::String relocUsedSection;
        bool relocIsExtern;

        bool canOptimize = false;

        bool useImmediate = false;
        Parser::Immediate immediate;
        struct {
            uint64_t max;
            uint16_t sizeInBits;

            uint64_t value;
        } immediateSpecific;
    };

    class Simple_Stack_Instruction : public Instruction
    {
    public:
        Simple_Stack_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Stack_Instruction() override {}
    };
}
