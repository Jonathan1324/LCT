#include "interrupt.hpp"

#include <x86/Instructions.hpp>
#include "x86.hpp"

x86::Argument_Interrupt_Instruction::Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
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

void x86::Argument_Interrupt_Instruction::evaluate()
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

    buffer.push_back(opcode);
    buffer.push_back(argument_value);
}

uint64_t x86::Argument_Interrupt_Instruction::size()
{
    return 2;
}


x86::Simple_Interrupt_Instruction::Simple_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic)
    : ::x86::Instruction(e)
{
    switch (mnemonic)
    {
        case Instructions::IRET:
            opcode = 0xCF;

            break;

        case Instructions::IRETQ:
            if (bits == BitMode::Bits16) throw Exception::SyntaxError("'iretq' not supported in 16-bit mode", -1, -1, nullptr);
            if (bits == BitMode::Bits32) throw Exception::SyntaxError("'iretq' not supported in 32-bit mode", -1, -1, nullptr);

            opcode = 0xCF;

            rex.use = true;
            rex.w = true;

            break;

        case Instructions::IRETD:
            if (bits == BitMode::Bits16) use16BitPrefix = true;

            opcode = 0xCF;

            break;

        case Instructions::SYSCALL:
            opcode = 0x05;

            useOpcodeEscape = true;

            break;

        case Instructions::SYSRET:
            opcode = 0x07;

            useOpcodeEscape = true;

            break;

        case Instructions::SYSENTER:
            opcode = 0x34;

            useOpcodeEscape = true;

            break;

        case Instructions::SYSEXIT:
            opcode = 0x35;

            useOpcodeEscape = true;

            break;

        default:
            throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
    }
}

void x86::Simple_Interrupt_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (use16BitPrefix) buffer.push_back(prefix16Bit);
    if (rex.use) buffer.push_back(getRex(rex.w, rex.r, rex.x, rex.b));

    if (useOpcodeEscape) buffer.push_back(opcodeEscape);
            
    buffer.push_back(opcode);
}

uint64_t x86::Simple_Interrupt_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;
    if (rex.use) s++;

    if (useOpcodeEscape) s++;

    return s;
}
