#include "interrupt.hpp"

#include <x86/Instructions.hpp>
#include "x86.hpp"

x86::Argument_Interrupt_Instruction::Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::Encoder::Encoder::Instruction(e)
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
        argument_value = static_cast<uint8_t>(evaluation.offset); // TODO: Check for overflow

        AddRelocation(
            1, // opcode
            evaluation.offset,
            true,
            evaluation.usedSection,
            ::Encoder::RelocationType::Absolute,
            ::Encoder::RelocationSize::Bit8,
            evaluation.isExtern
        );
    }
    else
    {
        Int128 result = evaluation.result;

        if (result < 0)   throw Exception::SemanticError("'int' can't have a negative operand", -1, -1);
        if (result > 255) throw Exception::SemanticError("Operand too large for 'int'", -1, -1);

        argument_value = static_cast<uint8_t>(result);
    }
}

std::vector<uint8_t> x86::Argument_Interrupt_Instruction::encode()
{
    return {opcode, argument_value};
}

uint64_t x86::Argument_Interrupt_Instruction::size()
{
    return 2;
}


x86::Simple_Interrupt_Instruction::Simple_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic)
    : ::Encoder::Encoder::Instruction(e)
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

            useREX = true;
            rexW = true;

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

std::vector<uint8_t> x86::Simple_Interrupt_Instruction::encode()
{
    std::vector<uint8_t> instr;

    if (use16BitPrefix) instr.push_back(prefix16Bit);
    if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));

    if (useOpcodeEscape) instr.push_back(opcodeEscape);
            
    instr.push_back(opcode);

    return instr;
}

uint64_t x86::Simple_Interrupt_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;
    if (useREX) s++;

    if (useOpcodeEscape) s++;

    return s;
}
