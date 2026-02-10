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

            if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
            {
                Parser::Instruction::Register destReg = std::get<Parser::Instruction::Register>(destinationOperand);

                switch (destReg.reg)
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

                    case CR8:
                    case CR9:
                    case CR10:
                    case CR11:
                    case CR12:
                    case CR13:
                    case CR14:
                    case CR15:

                    case DR8:
                    case DR9:
                    case DR10:
                    case DR11:
                    case DR12:
                    case DR13:
                    case DR14:
                    case DR15:
                        if (bits != BitMode::Bits64)
                            throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                }

                if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                {
                    Parser::Instruction::Register srcReg = std::get<Parser::Instruction::Register>(sourceOperand);

                    switch (srcReg.reg)
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

                        case CR8:
                        case CR9:
                        case CR10:
                        case CR11:
                        case CR12:
                        case CR13:
                        case CR14:
                        case CR15:

                        case DR8:
                        case DR9:
                        case DR10:
                        case DR11:
                        case DR12:
                        case DR13:
                        case DR14:
                        case DR15:
                            if (bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                    }

                    movType = MovType::MOV_REG_REG;

                    modrm.mod = ::x86::Mod::REGISTER;

                    modrm.use = true;

                    // FIXME: not only for non special regs
                    auto [destI, destUseREX, destSetREX] = ::x86::getReg(destReg.reg);
                    auto [srcI, srcUseREX, srcSetREX] = ::x86::getReg(srcReg.reg);

                    if (destUseREX || srcUseREX)
                        rex.use = true;
                    if (destSetREX)
                        rex.b = true;
                    if (srcSetREX)
                        rex.r = true;

                    uint8_t destRegSize = getRegSize(destReg.reg, bits);
                    uint8_t srcRegSize = getRegSize(srcReg.reg, bits);

                    if (destRegSize != srcRegSize)
                        throw Exception::SemanticError("Can't use 'mov' with registers of different size", -1, -1);

                    bool usingSpecialRegDest = false;
                    bool usingSpecialRegSrc = false;

                    switch (destReg.reg)
                    {
                        case ES:
                        case CS:
                        case SS:
                        case DS:
                        case FS:
                        case GS:
                            opcode = 0x8E;

                            modrm.reg = destI;
                            modrm.rm = srcI;

                            usingSpecialRegDest = true;
                            break;

                        case CR0:
                        case CR2:
                        case CR3:
                        case CR4:
                        case CR5:
                        case CR6:
                        case CR7:
                            useOpcodeEscape = true;
                            opcode = 0x22;

                            modrm.reg = destI;
                            modrm.rm = srcI;

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

                            modrm.reg = destI;
                            modrm.rm = srcI;

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

                            modrm.reg = destI;
                            modrm.rm = srcI;

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

                            modrm.reg = destI;
                            modrm.rm = srcI;

                            usingSpecialRegDest = true;
                            break;

                        case TR0:
                        case TR1:
                        case TR2:
                        case TR3:
                        case TR4:
                        case TR5:
                        case TR6:
                        case TR7:
                            useOpcodeEscape = true;
                            opcode = 0x26;

                            modrm.reg = destI;
                            modrm.rm = srcI;

                            usingSpecialRegDest = true;
                            break;
                    }

                    switch (destReg.reg)
                    {
                        case ES:
                        case CS:
                        case SS:
                        case DS:
                        case FS:
                        case GS:
                            opcode = 0x8C;

                            modrm.reg = srcI;
                            modrm.rm = destI;

                            usingSpecialRegSrc = true;
                            break;

                        case CR0:
                        case CR2:
                        case CR3:
                        case CR4:
                        case CR5:
                        case CR6:
                        case CR7:
                            useOpcodeEscape = true;
                            opcode = 0x20;

                            modrm.reg = srcI;
                            modrm.rm = destI;

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

                            modrm.reg = srcI;
                            modrm.rm = destI;

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

                            modrm.reg = srcI;
                            modrm.rm = destI;

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

                            modrm.reg = srcI;
                            modrm.rm = destI;

                            usingSpecialRegSrc = true;
                            break;

                        case TR0:
                        case TR1:
                        case TR2:
                        case TR3:
                        case TR4:
                        case TR5:
                        case TR6:
                        case TR7:
                            useOpcodeEscape = true;
                            opcode = 0x24;

                            modrm.reg = srcI;
                            modrm.rm = destI;

                            usingSpecialRegSrc = true;
                            break;
                    }

                    if (usingSpecialRegDest || usingSpecialRegSrc)
                        throw Exception::SemanticError("Can't set special register using special register", -1, -1);

                    const bool usingSpecialReg = usingSpecialRegDest || usingSpecialRegSrc;

                    if (!usingSpecialReg)
                    {
                        switch (destReg.reg)
                        {
                            case AL:
                            case CL:
                            case DL:
                            case BL:
                            case AH:
                            case CH:
                            case DH:
                            case BH:
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
                                opcode = 0x88; // mov r/m8, r8

                                modrm.reg = srcI;
                                modrm.rm = destI;
                                break;

                            case AX:
                            case CX:
                            case DX:
                            case BX:
                            case SP:
                            case BP:
                            case SI:
                            case DI:
                            case R8W:
                            case R9W:
                            case R10W:
                            case R11W:
                            case R12W:
                            case R13W:
                            case R14W:
                            case R15W:
                                if (bits != BitMode::Bits16)
                                    use16BitPrefix = true;
                                opcode = 0x89; // mov r/m16, r16

                                modrm.reg = srcI;
                                modrm.rm = destI;
                                break;

                            case EAX:
                            case ECX:
                            case EDX:
                            case EBX:
                            case ESP:
                            case EBP:
                            case ESI:
                            case EDI:
                            case R8D:
                            case R9D:
                            case R10D:
                            case R11D:
                            case R12D:
                            case R13D:
                            case R14D:
                            case R15D:
                                if (bits == BitMode::Bits16)
                                    use16BitPrefix = true;
                                opcode = 0x89; // mov r/m32, r32

                                modrm.reg = srcI;
                                modrm.rm = destI;
                                break;

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
                                opcode = 0x89; // mov r/m64, r64

                                rex.use = true;
                                rex.w = true;

                                modrm.reg = srcI;
                                modrm.rm = destI;
                                break;

                            default:
                                throw Exception::SemanticError("instruction doesn't support this register", -1, -1);
                        }
                    }

                    if (rex.use && (destReg.reg == AH || destReg.reg == CH ||
                                    destReg.reg == DH || destReg.reg == BH ||
                                    srcReg.reg == AH || srcReg.reg == CH ||
                                    srcReg.reg == DH || srcReg.reg == BH))
                        throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                {
                    throw Exception::InternalError("Memory not supported yet with 'mov'", -1, -1);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                {
                    movType = MovType::MOV_REG_IMM;

                    // TODO
                    switch (destReg.reg)
                    {
                        case AL:
                        case CL:
                        case DL:
                        case BL:
                        case AH:
                        case CH:
                        case DH:
                        case BH:
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
                            specific.mov_reg_imm.max = std::numeric_limits<uint8_t>::max();
                            specific.mov_reg_imm.sizeInBits = 8;
                            break;

                        case AX:
                        case CX:
                        case DX:
                        case BX:
                        case SP:
                        case BP:
                        case SI:
                        case DI:
                        case R8W:
                        case R9W:
                        case R10W:
                        case R11W:
                        case R12W:
                        case R13W:
                        case R14W:
                        case R15W:
                            specific.mov_reg_imm.max = std::numeric_limits<uint16_t>::max();
                            specific.mov_reg_imm.sizeInBits = 16;

                            if (bits != BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

                        case EAX:
                        case ECX:
                        case EDX:
                        case EBX:
                        case ESP:
                        case EBP:
                        case ESI:
                        case EDI:
                        case R8D:
                        case R9D:
                        case R10D:
                        case R11D:
                        case R12D:
                        case R13D:
                        case R14D:
                        case R15D:
                            specific.mov_reg_imm.max = std::numeric_limits<uint32_t>::max();
                            specific.mov_reg_imm.sizeInBits = 32;

                            if (bits == BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

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
                            specific.mov_reg_imm.max = std::numeric_limits<uint64_t>::max();
                            specific.mov_reg_imm.sizeInBits = 64;
                            break;

                        default:
                            throw Exception::SemanticError("instruction doesn't support this register", -1, -1);
                    }

                    switch (destReg.reg)
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

                    switch (destReg.reg)
                    {
                        case AL:
                            opcode = 0xB0;
                            break;
                        case CL:
                            opcode = 0xB1;
                            break;
                        case DL:
                            opcode = 0xB2;
                            break;
                        case BL:
                            opcode = 0xB3;
                            break;
                        case AH:
                            opcode = 0xB4;
                            break;
                        case CH:
                            opcode = 0xB5;
                            break;
                        case DH:
                            opcode = 0xB6;
                            break;
                        case BH:
                            opcode = 0xB7;
                            break;
                        case SPL:
                            opcode = 0xB4;
                            break;
                        case BPL:
                            opcode = 0xB5;
                            break;
                        case SIL:
                            opcode = 0xB6;
                            break;
                        case DIL:
                            opcode = 0xB7;
                            break;
                        case R8B:
                            opcode = 0xB0;
                            break;
                        case R9B:
                            opcode = 0xB1;
                            break;
                        case R10B:
                            opcode = 0xB2;
                            break;
                        case R11B:
                            opcode = 0xB3;
                            break;
                        case R12B:
                            opcode = 0xB4;
                            break;
                        case R13B:
                            opcode = 0xB5;
                            break;
                        case R14B:
                            opcode = 0xB6;
                            break;
                        case R15B:
                            opcode = 0xB7;
                            break;

                        case AX:
                            opcode = 0xB8;
                            break;
                        case CX:
                            opcode = 0xB9;
                            break;
                        case DX:
                            opcode = 0xBA;
                            break;
                        case BX:
                            opcode = 0xBB;
                            break;
                        case SP:
                            opcode = 0xBC;
                            break;
                        case BP:
                            opcode = 0xBD;
                            break;
                        case SI:
                            opcode = 0xBE;
                            break;
                        case DI:
                            opcode = 0xBF;
                            break;
                        case R8W:
                            opcode = 0xB8;
                            break;
                        case R9W:
                            opcode = 0xB9;
                            break;
                        case R10W:
                            opcode = 0xBA;
                            break;
                        case R11W:
                            opcode = 0xBB;
                            break;
                        case R12W:
                            opcode = 0xBC;
                            break;
                        case R13W:
                            opcode = 0xBD;
                            break;
                        case R14W:
                            opcode = 0xBE;
                            break;
                        case R15W:
                            opcode = 0xBF;
                            break;

                        case EAX:
                            opcode = 0xB8;
                            break;
                        case ECX:
                            opcode = 0xB9;
                            break;
                        case EDX:
                            opcode = 0xBA;
                            break;
                        case EBX:
                            opcode = 0xBB;
                            break;
                        case ESP:
                            opcode = 0xBC;
                            break;
                        case EBP:
                            opcode = 0xBD;
                            break;
                        case ESI:
                            opcode = 0xBE;
                            break;
                        case EDI:
                            opcode = 0xBF;
                            break;
                        case R8D:
                            opcode = 0xB8;
                            break;
                        case R9D:
                            opcode = 0xB9;
                            break;
                        case R10D:
                            opcode = 0xBA;
                            break;
                        case R11D:
                            opcode = 0xBB;
                            break;
                        case R12D:
                            opcode = 0xBC;
                            break;
                        case R13D:
                            opcode = 0xBD;
                            break;
                        case R14D:
                            opcode = 0xBE;
                            break;
                        case R15D:
                            opcode = 0xBF;
                            break;

                        case RAX:
                            opcode = 0xB8;
                            break;
                        case RCX:
                            opcode = 0xB9;
                            break;
                        case RDX:
                            opcode = 0xBA;
                            break;
                        case RBX:
                            opcode = 0xBB;
                            break;
                        case RSP:
                            opcode = 0xBC;
                            break;
                        case RBP:
                            opcode = 0xBD;
                            break;
                        case RSI:
                            opcode = 0xBE;
                            break;
                        case RDI:
                            opcode = 0xBF;
                            break;
                        case R8:
                            opcode = 0xB8;
                            break;
                        case R9:
                            opcode = 0xB9;
                            break;
                        case R10:
                            opcode = 0xBA;
                            break;
                        case R11:
                            opcode = 0xBB;
                            break;
                        case R12:
                            opcode = 0xBC;
                            break;
                        case R13:
                            opcode = 0xBD;
                            break;
                        case R14:
                            opcode = 0xBE;
                            break;
                        case R15:
                            opcode = 0xBF;
                            break;

                        default:
                            throw Exception::InternalError("Unknown register", -1, -1);
                    }
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
            {
                // TODO
                throw Exception::InternalError("Memory not supported yet with 'mov'", -1, -1);

                if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                {
                    // TODO
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
                if (result > specific.mov_reg_imm.max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

                specific.mov_reg_imm.value = static_cast<uint64_t>(result);
            }

            break;
        }
    }
}

bool x86::Mov_Instruction::optimizeS()
{
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
