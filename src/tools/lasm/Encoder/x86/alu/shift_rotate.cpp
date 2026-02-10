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

            if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
            {
                Parser::Instruction::Register mainReg = std::get<Parser::Instruction::Register>(mainOperand);

                switch (mainReg.reg)
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

                if (std::holds_alternative<Parser::Instruction::Register>(countOperand))
                {
                    Parser::Instruction::Register countReg = std::get<Parser::Instruction::Register>(countOperand);
                    
                    if (countReg.reg != Registers::CL)
                        throw Exception::InternalError("2 operand of shift/rotate instruction needs to be CL", -1, -1);

                    modrm.mod = Mod::REGISTER;

                    modrm.use = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

                    if (mainUseREX) rex.use = true;
                    if (mainSetREX) rex.b = true;

                    modrm.rm = mainI;

                    if (mainRegSize == 8) opcode = 0xD2;
                    else                  opcode = 0xD3;

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

                    switch (mainRegSize)
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
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(countOperand))
                {
                    throw Exception::InternalError("Memory not supported yet'", -1, -1);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(countOperand))
                {
                    usesImmediate = true;

                    modrm.mod = Mod::REGISTER;

                    modrm.use = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

                    if (mainUseREX) rex.use = true;
                    if (mainSetREX) rex.b = true;

                    modrm.rm = mainI;

                    if (mainRegSize == 8) opcode = 0xC0;
                    else                  opcode = 0xC1;

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

                    switch (mainRegSize)
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
                }
            }

            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                // TODO
                throw Exception::InternalError("Memory not supported yet", -1, -1);

                if (std::holds_alternative<Parser::Instruction::Register>(countOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(countOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(countOperand))
                {
                    // TODO
                }
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
