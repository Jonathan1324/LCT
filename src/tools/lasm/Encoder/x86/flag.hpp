#pragma once

#include "Encoder.hpp"

namespace x86 {
    class Simple_Flag_Instruction : public ::x86::Instruction
    {
    public:
        Simple_Flag_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);
        ~Simple_Flag_Instruction() override {}

        void evaluate() override {}

        bool optimize() override {return false;} // TODO

        std::vector<uint8_t> encode() override;

        uint64_t size() override;
    };
}
