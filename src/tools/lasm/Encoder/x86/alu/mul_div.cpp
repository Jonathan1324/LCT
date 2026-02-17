#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Mul_Div_ALU_Instruction::Mul_Div_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::IMUL:
        {
            if (instr.operands.size() <= 0)
                throw Exception::InternalError("Wrong argument count for mul/div alu instruction", -1, -1);

            const Parser::Instruction::Operand& mainOperand = instr.operands[0];

            if (instr.operands.size() != 1)
            {
                if (!std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    throw Exception::SyntaxError("First Operand of 2/3 imul has to be a register", -1, -1);
                }
                Parser::Instruction::Register mainReg = std::get<Parser::Instruction::Register>(mainOperand);
                uint64_t mainSize = parseRegister(mainReg, bits, true);

                if (!isGPR(mainReg.reg))
                    throw Exception::SemanticError("Mul/Div ALU instruction only accepts GPRs", -1, -1);

                if (instr.operands.size() == 2)
                {
                    const Parser::Instruction::Operand& secondOperand = instr.operands[1];

                    uint64_t secondSize;
                    if (std::holds_alternative<Parser::Instruction::Register>(secondOperand))
                    {
                        Parser::Instruction::Register secondReg = std::get<Parser::Instruction::Register>(secondOperand);
                        secondSize = parseRegister(secondReg, bits, false);

                        if (!isGPR(secondReg.reg))
                            throw Exception::SemanticError("Mul/Div ALU instruction only accepts GPRs", -1, -1);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(secondOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(secondOperand);
                        secondSize = parseMemory(mem, bits, false);

                        if (secondSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                            secondSize = mainSize;
                    }
                    else
                    {
                        throw Exception::InternalError("Immediate not allowed for 2 operand imul instruction", -1, -1);
                    }

                    if (secondSize != mainSize)
                        throw Exception::SemanticError("Operand sizes don't match", -1, -1);

                    opcodeEscape = OpcodeEscape::TWO_BYTE;
                    
                    opcode = 0xAF;

                    if (mainSize != secondSize)
                        throw Exception::SemanticError("Can't use instruction with operands of different size", -1, -1);

                    switch (mainSize)
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
                            throw Exception::SemanticError("Can't use 2 operand imul with 8 bit registers", -1, -1);
                            break;

                        default:
                            throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                    }
                }
                else if (instr.operands.size() == 3)
                {
                    const Parser::Instruction::Operand& secondOperand = instr.operands[1];

                    uint64_t secondSize;
                    if (std::holds_alternative<Parser::Instruction::Register>(secondOperand))
                    {
                        Parser::Instruction::Register secondReg = std::get<Parser::Instruction::Register>(secondOperand);
                        secondSize = parseRegister(secondReg, bits, false);

                        if (!isGPR(secondReg.reg))
                            throw Exception::SemanticError("Mul/Div ALU instruction only accepts GPRs", -1, -1);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(secondOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(secondOperand);
                        secondSize = parseMemory(mem, bits, false);

                        if (secondSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                            secondSize = mainSize;
                    }
                    else
                    {
                        throw Exception::InternalError("Immediate as seconds operand not allowed for 3 operand imul instruction", -1, -1);
                    }

                    if (secondSize != mainSize)
                        throw Exception::SemanticError("Operand sizes don't match", -1, -1);

                    immediate.use = true;

                    if (!std::holds_alternative<Parser::Immediate>(instr.operands[2]))
                        throw Exception::InternalError("imul 3 operands: third operands needs to be an immediate", -1, -1);
                    
                    immediate.immediate = std::get<Parser::Immediate>(instr.operands[2]);

                    opcode = 0x69;
                    canOptimize = true;

                    if (mainSize != secondSize)
                        throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                    switch (mainSize)
                    {
                        case 16:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;

                            immediate.sizeInBits = 16;

                            break;
                        
                        case 32:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;

                            immediate.sizeInBits = 32;

                            break;

                        case 64:
                            immediate.sizeInBits = 32;

                            rex.use = true;
                            rex.w = true;
                            break;

                        case 8:
                            throw Exception::SemanticError("Can't use 3 operand imul with 8 bit registers", -1, -1);
                            break;

                        default:
                            throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                    }
                }
                else
                {
                    throw Exception::InternalError("Wrong argument count for imul alu instruction", -1, -1);
                }

                break;
            }

            [[fallthrough]];
        }

        case Instructions::MUL: case Instructions::DIV: case Instructions::IDIV:
        {
            if (instr.operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for mul/div alu instruction", -1, -1);

            const Parser::Instruction::Operand& mainOperand = instr.operands[0];

            uint64_t size = Parser::Instruction::Memory::NO_POINTER_SIZE;
            if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                size = parseRegister(reg, bits, false);

                if (!isGPR(reg.reg))
                    throw Exception::SemanticError("Mul/Div ALU instruction only accepts GPRs", -1, -1);
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                size = parseMemory(mem, bits, true);
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
                    throw Exception::InternalError("Unknown size", -1, -1);
            }

            if (size == 8) opcode = 0xF6;
            else           opcode = 0xF7;

            switch (instr.mnemonic)
            {
                case MUL:  modrm.reg = 4; break;
                case IMUL: modrm.reg = 5; break;
                case DIV:  modrm.reg = 6; break;
                case IDIV: modrm.reg = 7; break;
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown two argument alu instruction", -1, -1, nullptr);
    }
}

bool x86::Mul_Div_ALU_Instruction::optimizeS()
{
    if (canOptimize && !immediate.needsRelocation)
    {
        int32_t value = static_cast<int32_t>(static_cast<uint32_t>(immediate.value));
        if (
            value <= static_cast<int64_t>(std::numeric_limits<int8_t>::max()) &&
            value >= static_cast<int64_t>(std::numeric_limits<int8_t>::min())
        )
        {
            canOptimize = false;

            opcode = 0x6B;

            immediate.sizeInBits = 8;

            return true;
        }
    }

    return false;
}
