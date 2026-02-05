#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Stack_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Stack_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Stack_Instruction() override {}

        void evaluate() override {}

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

    private:
        uint8_t opcode;

        bool use16BitPrefix = false;
    };
}
