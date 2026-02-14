#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Control_Instruction : public x86::Instruction
    {
    public:
        Simple_Control_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Control_Instruction() override {}
    };

    class JMP_Instruction : public Instruction
    {
    public:
        JMP_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~JMP_Instruction() override {}

        void evaluateS() override;
        bool optimizeS() override;
        void encodeS(std::vector<uint8_t>& buffer) override;
        uint64_t sizeS() override;

    private:
        uint64_t mnemonicI;

        bool useImmediate = false;
        Parser::Immediate immediate;

        bool usedReloc = false;
        std::string relocUsedSection;
        bool relocIsExtern;

        uint64_t value;

        uint64_t max;
        uint16_t sizeInBits;

        uint64_t instrSize;

        bool canOptimize = false;
    };

    class RET_Instruction : public Instruction
    {
    public:
        RET_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~RET_Instruction() override {}

        void evaluateS() override;
        void encodeS(std::vector<uint8_t>& buffer) override;
        uint64_t sizeS() override;

    private:
        bool useImmediate = false;
        Parser::Immediate immediate;

        bool usedReloc = false;
        std::string relocUsedSection;
        bool relocIsExtern;

        uint64_t value;

        uint64_t max;
        uint16_t sizeInBits;

        uint64_t instrSize;

        bool canOptimize = false;
    };

    class CALL_Instruction : public Instruction
    {
    public:
        CALL_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands);
        ~CALL_Instruction() override {}

        void evaluateS() override;
        void encodeS(std::vector<uint8_t>& buffer) override;
        uint64_t sizeS() override;

    private:
        bool useImmediate = false;
        Parser::Immediate immediate;

        bool usedReloc = false;
        std::string relocUsedSection;
        bool relocIsExtern;

        uint64_t value;

        uint64_t max;
        uint16_t sizeInBits;

        uint64_t instrSize;

        bool canOptimize = false;
    };
}
