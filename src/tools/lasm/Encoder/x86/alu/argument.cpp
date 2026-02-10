#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Argument_ALU_Instruction::Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
{
    switch (mnemonic)
    {
        case Instructions::NOT: case Instructions::NEG:
        case Instructions::INC: case Instructions::DEC:
        {
            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for argument ALU instruction", -1, -1);

            Parser::Instruction::Operand operand = operands[0];

            if (std::holds_alternative<Parser::Instruction::Register>(operand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);

                switch (reg.reg)
                {
                    case SPL:
                    case BPL:
                    case SIL:
                    case DIL:
                    case R8B:
                    case R9B:
                    case R10B:
                    case R11B:
                    case R12B:
                    case R13B:
                    case R14B:
                    case R15B:
                    case R8W:
                    case R9W:
                    case R10W:
                    case R11W:
                    case R12W:
                    case R13W:
                    case R14W:
                    case R15W:
                    case R8D:
                    case R9D:
                    case R10D:
                    case R11D:
                    case R12D:
                    case R13D:
                    case R14D:
                    case R15D:
                    case RAX:
                    case RCX:
                    case RDX:
                    case RBX:
                    case RSP:
                    case RBP:
                    case RSI:
                    case RDI:
                    case R8:
                    case R9:
                    case R10:
                    case R11:
                    case R12:
                    case R13:
                    case R14:
                    case R15:
                        if (bits != BitMode::Bits64)
                            throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                }

                modrm.use = true;

                modrm.mod = Mod::REGISTER;

                auto [idx, regUseREX, regSetREX] = getReg(reg.reg);
                uint8_t regSize = getRegSize(reg.reg, bits);

                if (regUseREX) rex.use = true;
                if (regSetREX) rex.b = true;

                modrm.rm = idx;

                if (regSize == 8)
                {
                    if (mnemonic == Instructions::NOT || mnemonic == Instructions::NEG)
                        opcode = 0xF6;
                    else // INC, DEC
                        opcode = 0xFE;
                }
                else
                {
                    if (mnemonic == Instructions::NOT || mnemonic == Instructions::NEG)
                        opcode = 0xF7;
                    else // INC, DEC
                        opcode = 0xFF;
                }

                switch (mnemonic)
                {
                    case Instructions::NOT: modrm.reg = 2; break;
                    case Instructions::NEG: modrm.reg = 3; break;

                    case Instructions::INC: modrm.reg = 0; break;
                    case Instructions::DEC: modrm.reg = 1; break;
                }

                switch (regSize)
                {
                    case 16:
                        if (bits != BitMode::Bits16) use16BitPrefix = true;
                        break;
                    
                    case 32:
                        if (bits == BitMode::Bits16) use16BitPrefix = true;
                        break;

                    case 64:
                        rex.use = true;
                        rex.w = true;
                        break;

                    case 8:
                        break;

                    default:
                        throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                }

                if (bits != BitMode::Bits64 && regSize != 8 &&
                    (mnemonic == Instructions::INC || mnemonic == Instructions::DEC))
                {
                    modrm.use = false;

                    if (mnemonic == Instructions::INC)
                        opcode = 0x40 + idx;
                    else // DEC
                        opcode = 0x48 + idx;
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                
                parseMemory(mem, bits, Parser::Instruction::Memory::NO_POINTER_SIZE);

                if (mem.pointer_size == 8)
                {
                    if (mnemonic == Instructions::NOT || mnemonic == Instructions::NEG)
                        opcode = 0xF6;
                    else // INC, DEC
                        opcode = 0xFE;
                }
                else
                {
                    if (mnemonic == Instructions::NOT || mnemonic == Instructions::NEG)
                        opcode = 0xF7;
                    else // INC, DEC
                        opcode = 0xFF;
                }

                switch (mnemonic)
                {
                    case Instructions::NOT: modrm.reg = 2; break;
                    case Instructions::NEG: modrm.reg = 3; break;

                    case Instructions::INC: modrm.reg = 0; break;
                    case Instructions::DEC: modrm.reg = 1; break;
                }

                switch (mem.pointer_size)
                {
                    case 16:
                        if (bits != BitMode::Bits16) use16BitPrefix = true;
                        break;
                    
                    case 32:
                        if (bits == BitMode::Bits16) use16BitPrefix = true;
                        break;

                    case 64:
                        if (bits != BitMode::Bits64)
                                throw Exception::SemanticError("Can't use 64 bit size of memory in 16/32 bit", -1, -1);
                        rex.use = true;
                        rex.w = true;
                        break;

                    case 8:
                        break;

                    default:
                        throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument ALU instruction", -1, -1, nullptr);
    }
}
