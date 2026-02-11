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

            uint64_t size;
            if (std::holds_alternative<Parser::Instruction::Register>(operand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);
                size = parseRegister(reg, bits, false);

                if (!isGPR(reg.reg))
                    throw Exception::SemanticError("Argument ALU instruction only accepts GPRs", -1, -1);
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                size = parseMemory(mem, bits, true);
            }

            if (size == 8)
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

            switch (size)
            {
                case 16:
                    if (bits != BitMode::Bits16) use16BitPrefix = true;
                    break;
                
                case 32:
                    if (bits == BitMode::Bits16) use16BitPrefix = true;
                    break;

                case 64:
                    if (bits != BitMode::Bits64)
                            throw Exception::SemanticError("Can't use 64 bit size in 16/32 bit", -1, -1);
                    rex.use = true;
                    rex.w = true;
                    break;

                case 8:
                    break;

                default:
                    throw Exception::InternalError("Invalid size", -1, -1);
            }

            if (std::holds_alternative<Parser::Instruction::Register>(operand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);

                if (bits != BitMode::Bits64 && size != 8 &&
                    (mnemonic == Instructions::INC || mnemonic == Instructions::DEC))
                {
                    modrm.use = false;

                    if (mnemonic == Instructions::INC)
                        opcode = 0x40 + getRegIndex(reg);
                    else // DEC
                        opcode = 0x48 + getRegIndex(reg);
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument ALU instruction", -1, -1, nullptr);
    }
}
