#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Control_Instruction : public ::x86::Instruction
    {
    public:
        Simple_Control_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);

        ~Simple_Control_Instruction() override {}

        void evaluate() override;

        bool optimize() override;

        void encodeS(std::vector<uint8_t>& buffer) override;

        uint64_t sizeS() override;
    };
}
