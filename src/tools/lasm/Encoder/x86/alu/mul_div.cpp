#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Mul_Div_ALU_Instruction::Mul_Div_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    switch (mnemonic)
    {
        case Instructions::IMUL:
        {
            if (operands.size() <= 0)
                throw Exception::InternalError("Wrong argument count for mul/div alu instruction", -1, -1);

            mainOperand = operands[0];

            if (operands.size() != 1)
            {
                if (!std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    throw Exception::SyntaxError("First Operand of 2/3 imul has to be a register", -1, -1);
                }
                Parser::Instruction::Register mainReg = std::get<Parser::Instruction::Register>(mainOperand);
                uint64_t mainSize = parseRegister(mainReg, bits, true);

                if (!isGPR(mainReg.reg))
                    throw Exception::SemanticError("Mul/Div ALU instruction only accepts GPRs", -1, -1);

                if (operands.size() == 2)
                {
                    secondOperand = operands[1];

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

                    mulDivType = MulDivType::TwoOperands;

                    useOpcodeEscape = true;
                    
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
                else if (operands.size() == 3)
                {
                    secondOperand = operands[1];

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

                    mulDivType = MulDivType::ThreeOperands;

                    opcode = 0x69;
                    canOptimize = true;

                    if (mainSize != secondSize)
                        throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                    switch (mainSize)
                    {
                        case 16:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;

                            threeOperandsSpecific.max = std::numeric_limits<uint16_t>::max();
                            threeOperandsSpecific.sizeInBits = 16;

                            break;
                        
                        case 32:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;

                            threeOperandsSpecific.max = std::numeric_limits<uint32_t>::max();
                            threeOperandsSpecific.sizeInBits = 32;

                            break;

                        case 64:
                            threeOperandsSpecific.max = std::numeric_limits<uint32_t>::max();
                            threeOperandsSpecific.sizeInBits = 32;

                            rex.use = true;
                            rex.w = true;
                            break;

                        case 8:
                            throw Exception::SemanticError("Can't use 3 operand imul with 8 bit registers", -1, -1);
                            break;

                        default:
                            throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                    }

                    if (!std::holds_alternative<Parser::Immediate>(operands[2]))
                        throw Exception::InternalError("imul 3 operands: third operands needs to be an immediate", -1, -1);
                    thirdOperand = std::get<Parser::Immediate>(operands[2]);
                }
                else
                {
                    throw Exception::InternalError("Wrong argument count for imul alu instruction", -1, -1);
                }

                break;
            }

            // Same logic when one operand
        }

        case Instructions::MUL: case Instructions::DIV: case Instructions::IDIV:
        {
            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for mul/div alu instruction", -1, -1);

            mulDivType = MulDivType::Simple;

            mainOperand = operands[0];

            uint64_t size;
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

            switch (mnemonic)
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

void x86::Mul_Div_ALU_Instruction::evaluateS()
{
    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            ::Encoder::Evaluation evaluation = Evaluate(thirdOperand, false, 0);

            if (evaluation.useOffset)
            {
                usedReloc = true;
                evalUsedSection = evaluation.usedSection;
                evalIsExtern = evaluation.isExtern;
                threeOperandsSpecific.value = evaluation.offset;
            }
            else
            {
                Int128 result = evaluation.result;

                // FIXME
                if (result > threeOperandsSpecific.max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

                threeOperandsSpecific.value = static_cast<uint64_t>(result);
            }

            break;
        }
    }
}

bool x86::Mul_Div_ALU_Instruction::optimizeS()
{
    if (canOptimize && !usedReloc)
    {
        int32_t value = static_cast<int32_t>(static_cast<uint32_t>(threeOperandsSpecific.value));
        if (
            value <= static_cast<int64_t>(std::numeric_limits<int8_t>::max()) &&
            value >= static_cast<int64_t>(std::numeric_limits<int8_t>::min())
        )
        {
            canOptimize = false;

            opcode = 0x6B;

            threeOperandsSpecific.max = std::numeric_limits<uint8_t>::max();
            threeOperandsSpecific.sizeInBits = 8;

            return true;
        }
    }

    return false;
}

void x86::Mul_Div_ALU_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            uint32_t sizeInBytes = threeOperandsSpecific.sizeInBits / 8;

            uint64_t oldSize = buffer.size();
            buffer.resize(oldSize + sizeInBytes);

            std::memcpy(buffer.data() + oldSize, &threeOperandsSpecific.value, sizeInBytes);

            if (usedReloc)
            {
                uint64_t currentOffset = 1; // Opcode
                if (use16BitPrefix) currentOffset++;
                if (rex.use) currentOffset++;
                if (useOpcodeEscape) currentOffset++;
                if (modrm.use) currentOffset++;

                ::Encoder::RelocationSize relocSize;
                switch (threeOperandsSpecific.sizeInBits)
                {
                    case 8: relocSize = ::Encoder::RelocationSize::Bit8; break;
                    case 16: relocSize = ::Encoder::RelocationSize::Bit16; break;
                    case 32: relocSize = ::Encoder::RelocationSize::Bit32; break;
                    case 64: relocSize = ::Encoder::RelocationSize::Bit64; break;
                    default: throw Exception::InternalError("Unknown size in bits " + std::to_string(threeOperandsSpecific.sizeInBits), -1, -1);
                }

                AddRelocation(
                    currentOffset,
                    threeOperandsSpecific.value,
                    true,
                    evalUsedSection,
                    ::Encoder::RelocationType::Absolute,
                    relocSize,
                    false, // TODO: Check if signed
                    evalIsExtern
                );
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown aluType", -1, -1);
    }
}

uint64_t x86::Mul_Div_ALU_Instruction::sizeS()
{
    uint64_t s = 0;

    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            uint32_t sizeInBytes = threeOperandsSpecific.sizeInBits / 8;

            s += sizeInBytes;

            break;
        }

        default:
            throw Exception::InternalError("Unknown aluType", -1, -1);
    }

    return s;
}
