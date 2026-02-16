#include "interrupt.hpp"

x86::Simple_Interrupt_Instruction::Simple_Interrupt_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::INT3:
            opcode = 0xCC;
            break;

        case Instructions::INTO:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("INTO not supported in 64-bit mode", -1, -1, nullptr);
            opcode = 0xCE;
            break;

        case Instructions::INT1:
            opcode = 0xF1;
            break;

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

            opcodeEscape = OpcodeEscape::TWO_BYTE;

            break;

        case Instructions::SYSRET:
            opcode = 0x07;

            opcodeEscape = OpcodeEscape::TWO_BYTE;

            break;

        case Instructions::SYSENTER:
            opcode = 0x34;

            opcodeEscape = OpcodeEscape::TWO_BYTE;

            break;

        case Instructions::SYSEXIT:
            opcode = 0x35;

            opcodeEscape = OpcodeEscape::TWO_BYTE;

            break;

        default:
            throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
    }
}
