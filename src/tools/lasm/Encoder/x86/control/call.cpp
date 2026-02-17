#include "control.hpp"

#include <limits>
#include <cstring>

x86::CALL_Instruction::CALL_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    if (instr.mnemonic != CALL)
        throw Exception::InternalError("Unknown call instruction", -1, -1);

    if (instr.operands.size() != 1)
        throw Exception::InternalError("Wrong argument count for call instruction", -1, -1);

    const Parser::Instruction::Operand& operand = instr.operands[0];

    if (std::holds_alternative<Parser::Immediate>(operand))
    {
        immediate.use= true;
        immediate.immediate = std::get<Parser::Immediate>(operand);
        immediate.ripRelative = true;

        // TODO
        immediate.is_signed = true;

        switch (bits)
        {
            case BitMode::Bits16:
                immediate.sizeInBits = 16;
                break;

            case BitMode::Bits32: case BitMode::Bits64:
                immediate.sizeInBits = 32;
                break;
        }

        opcode = 0xE8;
    }
    else
    {
        opcode = 0xFF;

        uint64_t size = Parser::Instruction::Memory::NO_POINTER_SIZE;
        if (std::holds_alternative<Parser::Instruction::Register>(operand))
        {
            Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);
            size = parseRegister(reg, bits, false);

            if (!isGPR(reg.reg))
                throw Exception::SyntaxError("Can only jmp to GPRs", -1, -1);
        }
        else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
        {
            Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
            size = parseMemory(mem, bits, false);
        }

        switch (size)
        {
            case Parser::Instruction::Memory::NO_POINTER_SIZE:
                break;

            case 8:
                throw Exception::SemanticError("Can't jump to 8-bit registers", -1, -1);

            case 16:
                if (bits == BitMode::Bits32) use16BitPrefix = true;
                if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 16-bit registers in 32/64 bit mode", -1, -1);
                break;

            case 32:
                if (bits == BitMode::Bits16) use16BitPrefix = true;
                if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 32-bit registers in 16/64 bit mode", -1, -1);
                break;

            case 64:
                if (bits == BitMode::Bits64) break;
                throw Exception::SemanticError("Can't jump to 32-bit registers in 16/32 bit mode", -1, -1);
        }

        modrm.use = true;
        modrm.reg = 2;
    }
}
