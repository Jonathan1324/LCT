#include "control.hpp"

#include <x86/Instructions.hpp>

x86::Simple_Control_Instruction::Simple_Control_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic)
    : ::x86::Instruction(e)
{
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

std::vector<uint8_t> x86::Simple_Control_Instruction::encode()
{
    return {opcode};
}

uint64_t x86::Simple_Control_Instruction::size()
{
    return 1;
}
