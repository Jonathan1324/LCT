#include "stack.hpp"

x86::Simple_Stack_Instruction::Simple_Stack_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic)
    : ::x86::Instruction(e, bits)
{
    switch (mnemonic)
    {
        case Instructions::PUSHA:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'pusha' not supported in 64-bit mode", -1, -1, nullptr);

            opcode = 0x60;

            break;
                
        case Instructions::POPA:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'popa' not supported in 64-bit mode", -1, -1, nullptr);

            opcode = 0x61;

            break;

        case Instructions::PUSHAD:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'pushad' not supported in 64-bit mode", -1, -1, nullptr);

            opcode = 0x60;

            if (bits == BitMode::Bits16) use16BitPrefix = true;

            break;

        case Instructions::POPAD:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'popad' not supported in 64-bit mode", -1, -1, nullptr);

            opcode = 0x61;

            if (bits == BitMode::Bits16) use16BitPrefix = true;

            break;

                
        case Instructions::PUSHF:
            opcode = 0x9C;

            break;

        case Instructions::POPF:
            opcode = 0x9D;

            break;

        case Instructions::PUSHFD:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'pushfd' not supported in 64-bit mode", -1, -1, nullptr);
                    
            opcode = 0x9C;

            if (bits == BitMode::Bits16) use16BitPrefix = true;

            break;

        case Instructions::POPFD:
            if (bits == BitMode::Bits64) throw Exception::SyntaxError("'popfs' not supported in 64-bit mode", -1, -1, nullptr);
                    
            opcode = 0x9D;

            if (bits == BitMode::Bits16) use16BitPrefix = true;

            break;

        case Instructions::PUSHFQ:
            if (bits == BitMode::Bits16) throw Exception::SyntaxError("'pushfq' not supported in 16-bit mode", -1, -1, nullptr);
            if (bits == BitMode::Bits32) throw Exception::SyntaxError("'pushfq' not supported in 32-bit mode", -1, -1, nullptr);

            opcode = 0x9C;

            break;

        case Instructions::POPFQ:
            if (bits == BitMode::Bits16) throw Exception::SyntaxError("'pushfq' not supported in 16-bit mode", -1, -1, nullptr);
            if (bits == BitMode::Bits32) throw Exception::SyntaxError("'pushfq' not supported in 32-bit mode", -1, -1, nullptr);

            opcode = 0x9D;

            break;

        default:
            throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
    }
}
