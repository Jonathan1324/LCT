#include "interrupt.hpp"

x86::Simple_Interrupt_Instruction::Simple_Interrupt_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
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
