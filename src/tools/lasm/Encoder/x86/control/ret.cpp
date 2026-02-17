#include "control.hpp"

#include <cstring>

x86::RET_Instruction::RET_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    if (instr.mnemonic != RET)
        throw Exception::InternalError("Unknown ret instruction", -1, -1);

    if (instr.operands.size() == 0)
    {
        opcode = 0xC3;
    }
    else if (instr.operands.size() == 1)
    {
        opcode = 0xC2;

        if (!std::holds_alternative<Parser::Immediate>(instr.operands[0]))
            throw Exception::InternalError("Invalid argument type for ret", -1, -1);

        immediate.use = true;
        immediate.immediate = std::get<Parser::Immediate>(instr.operands[0]);

        immediate.sizeInBits = 16;
    }
    else
        throw Exception::InternalError("Invalid argument count for ret", -1, -1);
}
