#include "control.hpp"

x86::CALL_Instruction::CALL_Instruction(::Encoder::Encoder &e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    if (mnemonic != CALL)
        throw Exception::InternalError("Unknown call instruction", -1, -1);

    
}

void x86::CALL_Instruction::evaluateS()
{
    // TODO
}

void x86::CALL_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    // TODO
}

uint64_t x86::CALL_Instruction::sizeS()
{
    // TODO
    return 0;
}
