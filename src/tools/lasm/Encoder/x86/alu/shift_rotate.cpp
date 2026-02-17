#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Shift_Rotate_ALU_Instruction::Shift_Rotate_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::SHL: case Instructions::SHR:
        case Instructions::SAL: case Instructions::SAR:
        case Instructions::ROL: case Instructions::ROR:
        case Instructions::RCL: case Instructions::RCR:
        {
            if (instr.operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for Shift/Rotate ALU instruction", -1, -1);

            const Parser::Instruction::Operand& mainOperand = instr.operands[0];
            const Parser::Instruction::Operand& countOperand = instr.operands[1];

            uint64_t size = Parser::Instruction::Memory::NO_POINTER_SIZE;
            if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                size = parseRegister(reg, bits, false);

                if (!isGPR(reg.reg))
                    throw Exception::SemanticError("Shift/Rotate ALU instruction only accepts GPRs", -1, -1);
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                size = parseMemory(mem, bits, true);
            }

            if (size == 8) is8Bit = true;

            if (std::holds_alternative<Parser::Instruction::Register>(countOperand))
            {
                Parser::Instruction::Register countReg = std::get<Parser::Instruction::Register>(countOperand);
                
                if (countReg.reg != Registers::CL)
                    throw Exception::InternalError("2 operand of Shift/Rotate ALU instruction needs to be CL", -1, -1);

                if (size == 8) opcode = 0xD2;
                else           opcode = 0xD3;
            }
            else if (std::holds_alternative<Parser::Immediate>(countOperand))
            {
                immediate.use = true;
                immediate.immediate = std::get<Parser::Immediate>(countOperand);

                immediate.sizeInBits = 8;

                canOptimize = true;
                if (size == 8) opcode = 0xC0;
                else           opcode = 0xC1;
            }

            switch (instr.mnemonic)
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
                case 8:
                    break;

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

                default:
                    throw Exception::InternalError("Unknown mainRegSize", -1, -1);
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown shift/rotate alu instruction", -1, -1, nullptr);
    }
}

bool x86::Shift_Rotate_ALU_Instruction::optimizeS()
{
    if (canOptimize && !immediate.needsRelocation)
    {
        if (static_cast<uint8_t>(immediate.value) == 1)
        {
            canOptimize = false;

            immediate.use = false;

            if (is8Bit) opcode = 0xD0;
            else        opcode = 0xD1;
        }
    }

    return false;
}
