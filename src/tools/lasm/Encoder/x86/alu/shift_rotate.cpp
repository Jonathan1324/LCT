#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Shift_Rotate_ALU_Instruction::Shift_Rotate_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
{
    switch (mnemonic)
    {
        case Instructions::SHL: case Instructions::SHR:
        case Instructions::SAL: case Instructions::SAR:
        case Instructions::ROL: case Instructions::ROR:
        case Instructions::RCL: case Instructions::RCR:
        {
            if (operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for shift/rotate alu instruction", -1, -1);

            mainOperand = operands[0];
            countOperand = operands[1];

            uint64_t size;
            if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                size = parseRegister(reg, bits, Parser::Instruction::Memory::NO_POINTER_SIZE, false);
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                size = parseMemory(mem, bits, Parser::Instruction::Memory::NO_POINTER_SIZE);
            }

            if (std::holds_alternative<Parser::Instruction::Register>(countOperand))
            {
                Parser::Instruction::Register countReg = std::get<Parser::Instruction::Register>(countOperand);
                
                if (countReg.reg != Registers::CL)
                    throw Exception::InternalError("2 operand of shift/rotate instruction needs to be CL", -1, -1);

                if (size == 8) opcode = 0xD2;
                else                  opcode = 0xD3;
            }
            else if (std::holds_alternative<Parser::Immediate>(countOperand))
            {
                usesImmediate = true;

                if (size == 8) opcode = 0xC0;
                else           opcode = 0xC1;
            }

            switch (mnemonic)
            {
                case Instructions::ROL: modrm.reg = 0; break;
                case Instructions::ROR: modrm.reg = 1; break;
                case Instructions::RCL: modrm.reg = 2; break;
                case Instructions::RCR: modrm.reg = 3; break;
                case Instructions::SHL: case Instructions::SAL:
                    modrm.reg = 4; break;
                case Instructions::SHR: modrm.reg = 5; break;
                case Instructions::SAR: modrm.reg = 7; break;
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
                    rex.use = true;
                    rex.w = true;
                    break;

                case 8:
                    break;

                default:
                    throw Exception::InternalError("Unknown mainRegSize", -1, -1);
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown shift/rotate alu instruction", -1, -1, nullptr);
    }
}

void x86::Shift_Rotate_ALU_Instruction::evaluateS()
{
    if (usesImmediate)
    {
        Parser::Immediate imm = std::get<Parser::Immediate>(countOperand);

        ::Encoder::Evaluation evaluation = Evaluate(imm);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            relocUsedSection = evaluation.usedSection;
            relocIsExtern = evaluation.isExtern;
            count = static_cast<uint8_t>(evaluation.offset); // TODO: Check for overflow
        }
        else
        {
            Int128 result = evaluation.result;

            if (result < 0)   throw Exception::SemanticError("shift/rotate instruction can't have a negative operand", -1, -1);
            if (result > 255) throw Exception::SemanticError("Operand too large for shift/rotate instruction", -1, -1);

            count = static_cast<uint8_t>(result);
        }
    }
}

bool x86::Shift_Rotate_ALU_Instruction::optimizeS()
{
    return false;
}

void x86::Shift_Rotate_ALU_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (usesImmediate)
    {
        buffer.push_back(count);

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (modrm.use) currentOffset++;

            AddRelocation(
                currentOffset,
                static_cast<uint64_t>(count),
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

uint64_t x86::Shift_Rotate_ALU_Instruction::sizeS()
{
    uint64_t s = 0;

    if (usesImmediate) s++;

    return s;
}
