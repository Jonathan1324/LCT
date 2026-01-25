#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>

namespace x86 {
    class Simple_Flag_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Flag_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic) : ::Encoder::Encoder::Instruction(e) {
            switch (mnemonic)
            {
                case Instructions::CLC:
                    opcode = 0xF8;
                    break;

                case Instructions::STC:
                    opcode = 0xF9;
                    break;

                case Instructions::CMC:
                    opcode = 0xF5;
                    break;

                case Instructions::CLD:
                    opcode = 0xFC;
                    break;

                case Instructions::STD:
                    opcode = 0xFD;
                    break;

                case Instructions::CLI:
                    opcode = 0xFA;
                    break;

                case Instructions::STI:
                    opcode = 0xFB;
                    break;

                case Instructions::LAHF:
                    opcode = 0x9F;
                    break;

                case Instructions::SAHF:
                    opcode = 0x9E;
                    break;

                default:
                    throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
            }
        }

        ~Simple_Flag_Instruction() override {

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
