#include "flag.hpp"

x86::Simple_Flag_Instruction::Simple_Flag_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic)
    : ::x86::Instruction(e)
{
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

void x86::Simple_Flag_Instruction::evaluate()
{

}

bool x86::Simple_Flag_Instruction::optimize()
{
    return false;
}

void x86::Simple_Flag_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    buffer.push_back(opcode);
}

uint64_t x86::Simple_Flag_Instruction::size()
{
    return 1;
}
