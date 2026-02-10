#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Mul_Div_ALU_Instruction::Mul_Div_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
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
                    throw Exception::SyntaxError("2/3 Operand imul: first operand can't be memory", -1, -1);
                }
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

                auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

                if (mainUseREX) rex.use = true;
                if (mainSetREX) rex.r = true;

                if (operands.size() == 2)
                {
                    mulDivType = MulDivType::TwoOperands;

                    useOpcodeEscape = true;
                    
                    opcode = 0xAF;

                    secondOperand = operands[1];

                    if (std::holds_alternative<Parser::Instruction::Register>(secondOperand))
                    {
                        Parser::Instruction::Register secondReg = std::get<Parser::Instruction::Register>(secondOperand);

                        switch (secondReg.reg)
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

                        modrm.mod = Mod::REGISTER;

                        modrm.use = true;

                        auto [secondI, secondUseREX, secondSetREX] = ::x86::getReg(secondReg.reg);
                        uint8_t secondRegSize = getRegSize(secondReg.reg, bits);

                        if (secondUseREX) rex.use = true;
                        if (secondSetREX) rex.b = true;

                        if (mainRegSize != secondRegSize)
                            throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                        modrm.reg = mainI;
                        modrm.rm = secondI;

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
                                throw Exception::SemanticError("Can't use 2 operand imul with 8 bit registers", -1, -1);
                                break;

                            default:
                                throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                        }
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(secondOperand))
                    {
                        throw Exception::InternalError("Memory not supported yet", -1, -1);
                        // TODO
                    }
                    else
                    {
                        throw Exception::InternalError("Immediate not allowed for 2 operand imul instruction", -1, -1);
                    }
                }
                else if (operands.size() == 3)
                {
                    mulDivType = MulDivType::ThreeOperands;

                    opcode = 0x69;

                    secondOperand = operands[1];
                    
                    if (!std::holds_alternative<Parser::Immediate>(operands[2]))
                        throw Exception::InternalError("imul 3 operands: third operands needs to be an immediate", -1, -1);
                    thirdOperand = std::get<Parser::Immediate>(operands[2]);

                    if (std::holds_alternative<Parser::Instruction::Register>(secondOperand))
                    {
                        Parser::Instruction::Register secondReg = std::get<Parser::Instruction::Register>(secondOperand);

                        switch (secondReg.reg)
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

                        modrm.mod = Mod::REGISTER;

                        modrm.use = true;

                        auto [secondI, secondUseREX, secondSetREX] = ::x86::getReg(secondReg.reg);
                        uint8_t secondRegSize = getRegSize(secondReg.reg, bits);

                        if (secondUseREX) rex.use = true;
                        if (secondSetREX) rex.b = true;

                        if (mainRegSize != secondRegSize)
                            throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);
                        
                        modrm.reg = mainI;
                        modrm.rm = secondI;

                        switch (mainRegSize)
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
                                threeOperandsSpecific.max = std::numeric_limits<uint64_t>::max();
                                threeOperandsSpecific.sizeInBits = 64;

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
                    else if (std::holds_alternative<Parser::Instruction::Memory>(secondOperand))
                    {
                        throw Exception::InternalError("Memory not supported yet", -1, -1);
                        // TODO
                    }
                    else
                    {
                        throw Exception::InternalError("Immediate as seconds operand not allowed for 3 operand imul instruction", -1, -1);
                    }
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

                modrm.mod = Mod::REGISTER;

                modrm.use = true;

                auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);

                modrm.rm = mainI;

                if (mainUseREX) rex.use = true;
                if (mainSetREX) rex.b = true;

                uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

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

                if (mainRegSize == 8) opcode = 0xF6;
                else                  opcode = 0xF7;

                switch (mnemonic)
                {
                    case MUL:  modrm.reg = 4; break;
                    case IMUL: modrm.reg = 5; break;
                    case DIV:  modrm.reg = 6; break;
                    case IDIV: modrm.reg = 7; break;
                }
            }
            else
            {
                throw Exception::InternalError("Memory/Immediate not allowed for mul/div instruction", -1, -1);
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown two argument alu instruction", -1, -1, nullptr);
    }
}

void x86::Mul_Div_ALU_Instruction::evaluate()
{
    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            ::Encoder::Evaluation evaluation = Evaluate(thirdOperand);

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

bool x86::Mul_Div_ALU_Instruction::optimize()
{
    return false;
}

void x86::Mul_Div_ALU_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (useOpcodeEscape) buffer.push_back(opcodeEscape);

    buffer.push_back(opcode);

    if (modrm.use) buffer.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));

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
    uint64_t s = 1;

    if (useOpcodeEscape) s++;

    if (modrm.use) s++;

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
