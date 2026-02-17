#include "ascii.hpp"

#include <limits>

x86::ASCII_Instruction::ASCII_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    if (bits == BitMode::Bits64)
        throw Exception::SyntaxError("ASCII Instruction not supported in 64-bit mode", -1, -1);

    switch (instr.mnemonic)
    {
        case AAA:
            opcode = 0x37;
            break;

        case AAS:
            opcode = 0x3F;
            break;

        case AAD: case AAM:
        {
            if (instr.mnemonic == AAD) opcode = 0xD5;
            else                 opcode = 0xD4;

            if (instr.operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for ascii instruction", -1, -1);

            if (!std::holds_alternative<Parser::Immediate>(instr.operands[0]))
                throw Exception::InternalError("Only immediates are allowed for conditional jumps", -1, -1);

            immediate.use = true;
            immediate.immediate = std::get<Parser::Immediate>(instr.operands[0]);

            immediate.sizeInBits = 8;
        }
    }
}
