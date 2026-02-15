#include "control.hpp"

#include <cstring>

x86::RET_Instruction::RET_Instruction(::Encoder::Encoder &e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    if (mnemonic != RET)
        throw Exception::InternalError("Unknown ret instruction", -1, -1);

    if (operands.size() == 0)
    {
        opcode = 0xC3;
    }
    else if (operands.size() == 1)
    {
        opcode = 0xC2;

        if (!std::holds_alternative<Parser::Immediate>(operands[0]))
            throw Exception::InternalError("Invalid argument type for ret", -1, -1);

        useImmediate = true;
        immediate = std::get<Parser::Immediate>(operands[0]);
    }
    else
        throw Exception::InternalError("Invalid argument count for ret", -1, -1);
}

void x86::RET_Instruction::evaluateS()
{
    if (useImmediate)
    {
        ::Encoder::Evaluation evaluation = Evaluate(immediate, false, 0);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            relocUsedSection = evaluation.usedSection;
            relocIsExtern = evaluation.isExtern;
            value = static_cast<uint16_t>(evaluation.offset); // TODO: Check for overflow
        }
        else
        {
            Int128 result = evaluation.result;

            value = static_cast<uint16_t>(result);
        }
    }
}

void x86::RET_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (usedReloc)
    {
        AddRelocation(
            1, // opcode
            static_cast<uint64_t>(value),
            true,
            relocUsedSection,
            ::Encoder::RelocationType::Absolute,
            ::Encoder::RelocationSize::Bit16,
            false, // Not signed
            relocIsExtern
        );
    }

    if (useImmediate)
    {
        uint64_t oldSize = buffer.size();
        buffer.resize(oldSize + 2);

        std::memcpy(buffer.data() + oldSize, &value, 2);
    }
}

uint64_t x86::RET_Instruction::sizeS()
{
    if (useImmediate)
        return 2;
    else
        return 0;
}
