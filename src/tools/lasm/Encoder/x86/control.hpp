#pragma once

#include "../Encoder.hpp"

namespace x86 {
    class Simple_Control_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Control_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic);

        ~Simple_Control_Instruction() override {}

        void evaluate() override {}

        bool optimize() override {return false;} // TODO

        std::vector<uint8_t> encode() override;

        uint64_t size() override;

    private:
        uint8_t opcode;
    };
}
