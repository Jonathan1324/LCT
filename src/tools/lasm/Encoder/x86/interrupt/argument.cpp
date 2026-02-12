#include "interrupt.hpp"

x86::Argument_Interrupt_Instruction::Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    switch (mnemonic)
    {
        case Instructions::INT:
        {
            opcode = 0xCD;

            if (operands.empty())
                throw Exception::InternalError("'int' requires one argument", -1, -1, nullptr);
            if (!std::holds_alternative<Parser::Immediate>(operands[0]))
                throw Exception::InternalError("'int': wrong operand type", -1, -1, nullptr);

            // TODO: Check

            argument = std::get<Parser::Immediate>(operands[0]);

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument control instruction", -1, -1, nullptr);
    }
}

void x86::Argument_Interrupt_Instruction::evaluateS()
{
    ::Encoder::Evaluation evaluation = Evaluate(argument);

    if (evaluation.useOffset)
    {
        usedReloc = true;
        relocUsedSection = evaluation.usedSection;
        relocIsExtern = evaluation.isExtern;
        argument_value = static_cast<uint8_t>(evaluation.offset); // TODO: Check for overflow
    }
    else
    {
        Int128 result = evaluation.result;

        if (result < 0)   throw Exception::SemanticError("'int' can't have a negative operand", -1, -1);
        if (result > 255) throw Exception::SemanticError("Operand too large for 'int'", -1, -1);

        argument_value = static_cast<uint8_t>(result);
    }
}

void x86::Argument_Interrupt_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (usedReloc)
    {
        AddRelocation(
            1, // opcode
            static_cast<uint64_t>(argument_value),
            true,
            relocUsedSection,
            ::Encoder::RelocationType::Absolute,
            ::Encoder::RelocationSize::Bit8,
            false, // Not signed
            relocIsExtern
        );
    }

    buffer.push_back(argument_value);
}

uint64_t x86::Argument_Interrupt_Instruction::sizeS()
{
    return 1;
}
