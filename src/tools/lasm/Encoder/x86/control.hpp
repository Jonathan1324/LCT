#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>

namespace x86 {
    class Simple_Control_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Control_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic) : ::Encoder::Encoder::Instruction(e) {
            switch (mnemonic)
            {
                case Instructions::NOP:
                    opcode = 0x90;
                    break;

                case Instructions::HLT:
                    opcode = 0xF4;
                    break;

                default:
                    throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
            }
        }

        ~Simple_Control_Instruction() override {

        }

        void evaluate() override {}

        std::vector<uint8_t> encode() override {
            return {opcode};
        }

        uint64_t size() override {
            return 1;
        }

    private:
        uint8_t opcode;
    };
}
