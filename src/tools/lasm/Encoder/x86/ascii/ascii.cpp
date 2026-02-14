#include "ascii.hpp"

#include <limits>

x86::ASCII_Instruction::ASCII_Instruction(::Encoder::Encoder &e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    if (bits == BitMode::Bits64)
        throw Exception::SyntaxError("ASCII Instruction not supported in 64-bit mode", -1, -1);

    switch (mnemonic)
    {
        case AAA:
            opcode = 0x37;
            break;

        case AAS:
            opcode = 0x3F;
            break;

        case AAD: case AAM:
        {
            if (mnemonic == AAD) opcode = 0xD5;
            else                 opcode = 0xD4;

            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for ascii instruction", -1, -1);

            if (!std::holds_alternative<Parser::Immediate>(operands[0]))
                throw Exception::InternalError("Only immediates are allowed for conditional jumps", -1, -1);

            useImmediate = true;

            max = std::numeric_limits<uint8_t>::max();
            sizeInBits = 8;

            immediate = std::get<Parser::Immediate>(operands[0]);
        }
    }
}

void x86::ASCII_Instruction::evaluateS()
{
    if (useImmediate)
    {
        ::Encoder::Evaluation evaluation = Evaluate(immediate, false, 0);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            relocUsedSection = evaluation.usedSection;
            relocIsExtern = evaluation.isExtern;
            value = static_cast<uint8_t>(evaluation.offset);
        }
        else
        {
            Int128 result = evaluation.result;

            // FIXME
            //if (result > max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

            value = static_cast<uint8_t>(result);
        }
    }
}

void x86::ASCII_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (useImmediate)
    {
        buffer.push_back(value);

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (useOpcodeEscape) currentOffset++;
            if (modrm.use) currentOffset++;

            AddRelocation(
                currentOffset,
                value,
                true,
                relocUsedSection,
                ::Encoder::RelocationType::Absolute,
                ::Encoder::RelocationSize::Bit8,
                false, // TODO: Check if signed
                relocIsExtern
            );
        }
    }
}

uint64_t x86::ASCII_Instruction::sizeS()
{
    if (useImmediate)
        return 1;
    else
        return 0;
}
