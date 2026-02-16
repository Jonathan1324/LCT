#include "control.hpp"

x86::Simple_Control_Instruction::Simple_Control_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
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
