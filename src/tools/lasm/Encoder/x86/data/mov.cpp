#include "data.hpp"

#include <limits>
#include <cstring>

x86::Mov_Instruction::Mov_Instruction(::Encoder::Encoder &e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
{
    switch (mnemonic)
    {
        case Instructions::MOV:
        {
            if (operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for 'mov'", -1, -1, nullptr);

            destinationOperand = operands[0];
            sourceOperand = operands[1];

            bool usingSpecialReg = false;
            bool usingSegment = false;

            if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                if (!isGPR(reg.reg)) usingSpecialReg = true;
                if (isSegment(reg.reg)) usingSegment = true;
            }

            if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(sourceOperand);
                if (!isGPR(reg.reg)) usingSpecialReg = true;
                if (isSegment(reg.reg)) usingSegment = true;
            }

            if (usingSpecialReg && !usingSegment)
            {
                if (!std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    throw Exception::SyntaxError("Can only set GPRs to crX/drX/trX", -1, -1);

                if (!std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    throw Exception::SyntaxError("Can only set crX/drX/trX to GPRs", -1, -1);

                Parser::Instruction::Register destReg = std::get<Parser::Instruction::Register>(destinationOperand);
                Parser::Instruction::Register srcReg = std::get<Parser::Instruction::Register>(sourceOperand);

                movType = MovType::MOV_REG_REG;

                bool usingSpecialRegDest = false;
                bool usingSpecialRegSrc = false;

                switch (destReg.reg)
                    {
                    case CR0:
                    case CR2:
                    case CR3:
                    case CR4:
                    case CR5:
                    case CR6:
                    case CR7:
                        useOpcodeEscape = true;
                        opcode = 0x22;

                        usingSpecialRegDest = true;
                        break;
                    case CR8:
                    case CR9:
                    case CR10:
                    case CR11:
                    case CR12:
                    case CR13:
                    case CR14:
                    case CR15:
                        useOpcodeEscape = true;
                        opcode = 0x22;

                        rex.use = true;
                        rex.r = true;

                        usingSpecialRegDest = true;
                        break;

                    case DR0:
                    case DR1:
                    case DR2:
                    case DR3:
                    case DR6:
                    case DR7:
                        useOpcodeEscape = true;
                        opcode = 0x23;

                        usingSpecialRegDest = true;
                        break;
                    case DR8:
                    case DR9:
                    case DR10:
                    case DR11:
                    case DR12:
                    case DR13:
                    case DR14:
                    case DR15:
                        useOpcodeEscape = true;
                        opcode = 0x23;

                        rex.use = true;
                        rex.r = true;

                        usingSpecialRegDest = true;
                        break;
                    }

                switch (srcReg.reg)
                {
                    case CR0:
                    case CR2:
                    case CR3:
                    case CR4:
                    case CR5:
                    case CR6:
                    case CR7:
                        useOpcodeEscape = true;
                        opcode = 0x20;

                        usingSpecialRegSrc = true;
                        break;
                    case CR8:
                    case CR9:
                    case CR10:
                    case CR11:
                    case CR12:
                    case CR13:
                    case CR14:
                    case CR15:
                        useOpcodeEscape = true;
                        opcode = 0x20;

                        rex.use = true;
                        rex.r = true;

                        usingSpecialRegSrc = true;
                        break;

                    case DR0:
                    case DR1:
                    case DR2:
                    case DR3:
                    case DR6:
                    case DR7:
                        useOpcodeEscape = true;
                        opcode = 0x21;

                        usingSpecialRegSrc = true;
                        break;
                    case DR8:
                    case DR9:
                    case DR10:
                    case DR11:
                    case DR12:
                    case DR13:
                    case DR14:
                    case DR15:
                        useOpcodeEscape = true;
                        opcode = 0x21;

                        rex.use = true;
                        rex.r = true;

                        usingSpecialRegSrc = true;
                        break;
                }

                if (usingSpecialRegDest && usingSpecialRegSrc)
                    throw Exception::SemanticError("Can't set special register using special register", -1, -1);

                checkReg(destReg, bits);
                checkReg(srcReg, bits);

                modrm.use = true;

                auto [destI, destUseREX, destSetREX] = ::x86::getReg(destReg.reg);
                auto [srcI, srcUseREX, srcSetREX] = ::x86::getReg(srcReg.reg);

                if (destUseREX || srcUseREX)
                    rex.use = true;
                if (destSetREX || srcSetREX)
                    rex.b = true;

                uint8_t destRegSize = getRegSize(destReg.reg, bits);
                uint8_t srcRegSize = getRegSize(srcReg.reg, bits);

                if (destRegSize != srcRegSize)
                    throw Exception::SemanticError("Can't use 'mov' with registers of different size", -1, -1);

                if (usingSpecialRegDest)
                {
                    modrm.reg = destI;
                    modrm.rm = srcI;
                }
                else
                {
                    modrm.reg = srcI;
                    modrm.rm = destI;
                }
            }
            else if (usingSegment)
            {
                bool useRMFirst = false;
                if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                {
                    Parser::Instruction::Register destReg = std::get<Parser::Instruction::Register>(destinationOperand);

                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(sourceOperand);
                        if (isSegment(reg.reg) && isSegment(destReg.reg))
                            throw Exception::SyntaxError("Can't use mov with two segment registers", -1, -1);

                        if (isSegment(reg.reg))
                            useRMFirst = true;
                        else
                            useRMFirst = false;
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                    {
                        useRMFirst = false;
                    }
                    else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                    {
                        throw Exception::SyntaxError("Can't use immediates with mov with segment register", -1, -1);
                    }
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                {
                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        useRMFirst = true;
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                    {
                        throw Exception::SemanticError("Instruction can't have two memory operands", -1, -1);
                    }
                    else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                    {
                        throw Exception::SyntaxError("Can't use immediates with mov with segment register", -1, -1);
                    }
                }

                movType = MovType::MOV_REG_REG;

                uint64_t destSize;
                uint64_t srcSize;

                bool oneMem = false;

                if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                    destSize = parseRegister(reg, bits, !useRMFirst);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(destinationOperand);
                    destSize = parseMemory(mem, bits, false);

                    oneMem = true;
                }

                if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(sourceOperand);
                    srcSize = parseRegister(reg, bits, useRMFirst);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(sourceOperand);
                    srcSize = parseMemory(mem, bits, false);

                    oneMem = true;

                    if (srcSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                        srcSize = destSize;
                }

                if (destSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                    destSize = srcSize;

                if (oneMem && destSize != srcSize)
                    throw Exception::SyntaxError("Can only use memory operand of size 16 with mov with segment registers", -1, -1);

                opcode = (useRMFirst ? 0x8C : 0x8E);

                switch (destSize)
                {
                    case 16:
                        if (!oneMem && useRMFirst && bits != BitMode::Bits16) use16BitPrefix = true;
                        break;

                    case 32: case 64:
                        break;

                    default:
                        throw Exception::InternalError("Invalid operand size", -1, -1);
                }
            }
            else
            {
                bool useRMFirst = false;
                bool otherIsImmediate = false;
                bool oneMem = false;

                if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                {
                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        useRMFirst = true;
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                    {
                        useRMFirst = false;
                        oneMem = true;
                    }
                    else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                    {
                        otherIsImmediate = true;
                    }
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                {
                    oneMem = true;
                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        useRMFirst = true;
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                    {
                        throw Exception::SemanticError("Instruction can't have two memory operands", -1, -1);
                    }
                    else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                    {
                        otherIsImmediate = true;
                    }
                }

                if (otherIsImmediate)
                {
                    movType = MovType::MOV_REG_IMM;

                    uint64_t destSize;
                    if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                        destSize = getRegSize(reg.reg, bits);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(destinationOperand);
                        destSize = parseMemory(mem, bits, true);
                    }

                    switch (destSize)
                    {
                        case 8:
                            specific.mov_reg_imm.max = std::numeric_limits<uint8_t>::max();
                            specific.mov_reg_imm.sizeInBits = 8;

                            is_8_bit = true;
                            break;

                        case 16:
                            specific.mov_reg_imm.max = std::numeric_limits<uint16_t>::max();
                            specific.mov_reg_imm.sizeInBits = 16;

                            if (bits != BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

                        case 32:
                            specific.mov_reg_imm.max = std::numeric_limits<uint32_t>::max();
                            specific.mov_reg_imm.sizeInBits = 32;

                            if (bits == BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

                        case 64:
                            if (oneMem)
                            {
                                specific.mov_reg_imm.max = std::numeric_limits<uint32_t>::max();
                                specific.mov_reg_imm.sizeInBits = 32;
                            }
                            else
                            {
                                specific.mov_reg_imm.max = std::numeric_limits<uint64_t>::max();
                                specific.mov_reg_imm.sizeInBits = 64;
                            }

                            rex.use = true;
                            rex.w = true;

                            if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                            {
                                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                                can_optimize = true;
                                optimize_reg = reg;

                                auto [_, n, _] = getReg(reg.reg);
                                needs_rex = n;
                            }

                            break;

                        default:
                            throw Exception::InternalError("Invalid bits", -1, -1);
                    }

                    if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                        
                        switch (reg.reg)
                        {
                            case SPL:
                            case BPL:
                            case SIL:
                            case DIL:
                                rex.use = true;
                                break;

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
                                rex.use = true;
                                rex.b = true;
                                break;

                            case R8:
                            case R9:
                            case R10:
                            case R11:
                            case R12:
                            case R13:
                            case R14:
                            case R15:
                                rex.use = true;
                                rex.b = true;
                                rex.w = true;
                                break;

                            case RAX:
                            case RCX:
                            case RDX:
                            case RBX:
                            case RSP:
                            case RBP:
                            case RSI:
                            case RDI:
                                rex.use = true;
                                rex.w = true;
                                break;
                        }

                        switch (reg.reg)
                        {
                            case AL:   opcode = 0xB0; break;
                            case CL:   opcode = 0xB1; break;
                            case DL:   opcode = 0xB2; break;
                            case BL:   opcode = 0xB3; break;
                            case AH:   opcode = 0xB4; break;
                            case CH:   opcode = 0xB5; break;
                            case DH:   opcode = 0xB6; break;
                            case BH:   opcode = 0xB7; break;
                            case SPL:  opcode = 0xB4; break;
                            case BPL:  opcode = 0xB5; break;
                            case SIL:  opcode = 0xB6; break;
                            case DIL:  opcode = 0xB7; break;
                            case R8B:  opcode = 0xB0; break;
                            case R9B:  opcode = 0xB1; break;
                            case R10B: opcode = 0xB2; break;
                            case R11B: opcode = 0xB3; break;
                            case R12B: opcode = 0xB4; break;
                            case R13B: opcode = 0xB5; break;
                            case R14B: opcode = 0xB6; break;
                            case R15B: opcode = 0xB7; break;

                            case AX:   opcode = 0xB8; break;
                            case CX:   opcode = 0xB9; break;
                            case DX:   opcode = 0xBA; break;
                            case BX:   opcode = 0xBB; break;
                            case SP:   opcode = 0xBC; break;
                            case BP:   opcode = 0xBD; break;
                            case SI:   opcode = 0xBE; break;
                            case DI:   opcode = 0xBF; break;
                            case R8W:  opcode = 0xB8; break;
                            case R9W:  opcode = 0xB9; break;
                            case R10W: opcode = 0xBA; break;
                            case R11W: opcode = 0xBB; break;
                            case R12W: opcode = 0xBC; break;
                            case R13W: opcode = 0xBD; break;
                            case R14W: opcode = 0xBE; break;
                            case R15W: opcode = 0xBF; break;

                            case EAX:  opcode = 0xB8; break;
                            case ECX:  opcode = 0xB9; break;
                            case EDX:  opcode = 0xBA; break;
                            case EBX:  opcode = 0xBB; break;
                            case ESP:  opcode = 0xBC; break;
                            case EBP:  opcode = 0xBD; break;
                            case ESI:  opcode = 0xBE; break;
                            case EDI:  opcode = 0xBF; break;
                            case R8D:  opcode = 0xB8; break;
                            case R9D:  opcode = 0xB9; break;
                            case R10D: opcode = 0xBA; break;
                            case R11D: opcode = 0xBB; break;
                            case R12D: opcode = 0xBC; break;
                            case R13D: opcode = 0xBD; break;
                            case R14D: opcode = 0xBE; break;
                            case R15D: opcode = 0xBF; break;

                            case RAX:  opcode = 0xB8; break;
                            case RCX:  opcode = 0xB9; break;
                            case RDX:  opcode = 0xBA; break;
                            case RBX:  opcode = 0xBB; break;
                            case RSP:  opcode = 0xBC; break;
                            case RBP:  opcode = 0xBD; break;
                            case RSI:  opcode = 0xBE; break;
                            case RDI:  opcode = 0xBF; break;
                            case R8:   opcode = 0xB8; break;
                            case R9:   opcode = 0xB9; break;
                            case R10:  opcode = 0xBA; break;
                            case R11:  opcode = 0xBB; break;
                            case R12:  opcode = 0xBC; break;
                            case R13:  opcode = 0xBD; break;
                            case R14:  opcode = 0xBE; break;
                            case R15:  opcode = 0xBF; break;

                            default:
                                throw Exception::InternalError("Unknown register", -1, -1);
                        }
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                    {
                        if (is_8_bit) opcode = 0xC6;
                        else          opcode = 0xC7;
                    }
                }
                else
                {
                    movType = MovType::MOV_REG_REG;

                    uint64_t destSize;
                    uint64_t srcSize;

                    if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                        destSize = parseRegister(reg, bits, !useRMFirst);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(destinationOperand);
                        destSize = parseMemory(mem, bits, false);
                    }

                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(sourceOperand);
                        srcSize = parseRegister(reg, bits, useRMFirst);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(sourceOperand);
                        srcSize = parseMemory(mem, bits, false);

                        if (srcSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                            srcSize = destSize;
                    }

                    if (destSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                        destSize = srcSize;

                    if (destSize != srcSize)
                        throw Exception::SemanticError("Can't use instruction with operands of different size", -1, -1);

                    switch (destSize)
                    {
                        case 8:
                            opcode = (useRMFirst ? 0x88 : 0x8A);
                            break;

                        case 16:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;
                            opcode = (useRMFirst ? 0x89 : 0x8B);
                            break;

                        case 32:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;
                            opcode = (useRMFirst ? 0x89 : 0x8B);
                            break;

                        case 64:
                            opcode = (useRMFirst ? 0x89 : 0x8B);

                            rex.use = true;
                            rex.w = true;
                            break;

                        default:
                            throw Exception::InternalError("Invalid operand size", -1, -1);
                    }

                    if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(destinationOperand);
                        if (rex.use && (reg.reg == AH || reg.reg == CH ||
                                        reg.reg == DH || reg.reg == BH))
                            throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                    }
                    if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(sourceOperand);
                        if (rex.use && (reg.reg == AH || reg.reg == CH ||
                                        reg.reg == DH || reg.reg == BH))
                            throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                    }
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument control instruction", -1, -1, nullptr);
    }
}

void x86::Mov_Instruction::evaluateS()
{
    switch (movType)
    {
        case MovType::MOV_REG_REG: break;

        case MovType::MOV_REG_IMM:
        {
            Parser::Immediate srcImm = std::get<Parser::Immediate>(sourceOperand);

            ::Encoder::Evaluation evaluation = Evaluate(srcImm);

            if (evaluation.useOffset)
            {
                usedReloc = true;
                relocUsedSection = evaluation.usedSection;
                relocIsExtern = evaluation.isExtern;
                specific.mov_reg_imm.value = evaluation.offset;
            }
            else
            {
                Int128 result = evaluation.result;

                // FIXME
                //if (result > specific.mov_reg_imm.max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

                specific.mov_reg_imm.value = static_cast<uint64_t>(result);
            }

            break;
        }
    }
}

bool x86::Mov_Instruction::optimizeS()
{
    if (can_optimize && movType == MovType::MOV_REG_IMM)
    {
        int64_t value = static_cast<int64_t>(specific.mov_reg_imm.value);
        if (value <= static_cast<int64_t>(static_cast<uint64_t>(std::numeric_limits<uint32_t>().max())))
        {
            if (value >= 0)
            {
                if (!needs_rex) rex.use = false;
                rex.w = false;

                specific.mov_reg_imm.max = std::numeric_limits<uint32_t>::max();
                specific.mov_reg_imm.sizeInBits = 32;
                
                can_optimize = false;
                return true;
            }
            else if (value >= static_cast<int64_t>(static_cast<uint64_t>(std::numeric_limits<int32_t>().min())))
            {
                specific.mov_reg_imm.max = std::numeric_limits<uint32_t>::max();
                specific.mov_reg_imm.sizeInBits = 32;

                if (is_8_bit) opcode = 0xC6;
                else          opcode = 0xC7;

                modrm.use = true;
                modrm.mod = Mod::REGISTER;
                modrm.reg = 0;
                modrm.rm = getRegIndex(optimize_reg);
                
                can_optimize = false;
                return true;
            }
        }
    }

    return false;
}

void x86::Mov_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    switch (movType)
    {
        case MovType::MOV_REG_REG:
        {
            // TODO
            break;
        }

        case MovType::MOV_REG_IMM:
        {
            uint32_t sizeInBytes = specific.mov_reg_imm.sizeInBits / 8;

            uint64_t oldSize = buffer.size();
            buffer.resize(oldSize + sizeInBytes);

            std::memcpy(buffer.data() + oldSize, &specific.mov_reg_imm.value, sizeInBytes);

            if (usedReloc)
            {
                uint64_t currentOffset = 1; // opcode
                if (use16BitPrefix) currentOffset++;
                if (rex.use) currentOffset++;
                if (useOpcodeEscape) currentOffset++;
                if (modrm.use) currentOffset++;

                ::Encoder::RelocationSize relocSize;
                switch (specific.mov_reg_imm.sizeInBits)
                {
                    case 8: relocSize = ::Encoder::RelocationSize::Bit8; break;
                    case 16: relocSize = ::Encoder::RelocationSize::Bit16; break;
                    case 32: relocSize = ::Encoder::RelocationSize::Bit32; break;
                    case 64: relocSize = ::Encoder::RelocationSize::Bit64; break;
                    default: throw Exception::InternalError("Unknown size in bits " + std::to_string(specific.mov_reg_imm.sizeInBits), -1, -1);
                }

                AddRelocation(
                    currentOffset,
                    specific.mov_reg_imm.value,
                    true,
                    relocUsedSection,
                    ::Encoder::RelocationType::Absolute,
                    relocSize,
                    false, // TODO: Check if signed
                    relocIsExtern
                );
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown movType", -1, -1);
    }
}

uint64_t x86::Mov_Instruction::sizeS()
{
    uint64_t s = 0;

    switch (movType)
    {
        case MovType::MOV_REG_REG:
        {
            // TODO
            break;
        }

        case MovType::MOV_REG_IMM:
        {
            uint32_t sizeInBytes = specific.mov_reg_imm.sizeInBits / 8;

            s += sizeInBytes;

            break;
        }

        default:
            throw Exception::InternalError("Unknown movType", -1, -1);
    }

    return s;
}
