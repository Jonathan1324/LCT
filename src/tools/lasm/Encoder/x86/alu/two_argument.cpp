#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Two_Argument_ALU_Instruction::Two_Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e)
{
    switch (mnemonic)
    {
        case Instructions::ADD: case Instructions::ADC:
        case Instructions::SUB: case Instructions::SBB:
        case Instructions::CMP: case Instructions::TEST:
        case Instructions::AND: case Instructions::OR:
        case Instructions::XOR:
        {
            if (operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for two argument alu instruction", -1, -1);

            mainOperand = operands[0];
            otherOperand = operands[1];

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

                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    Parser::Instruction::Register otherReg = std::get<Parser::Instruction::Register>(otherOperand);

                    switch (otherReg.reg)
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
                            if (bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                    }

                    aluType = AluType::ALU_REG_REG;

                    modrm.mod = ::x86::Mod::REGISTER;

                    modrm.use = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    auto [otherI, otherUseREX, otherSetREX] = ::x86::getReg(otherReg.reg);

                    if (mainUseREX || otherUseREX)
                        rex.use = true;
                    if (mainSetREX)
                        rex.b = true;
                    if (otherSetREX)
                        rex.r = true;

                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);
                    uint8_t otherRegSize = getRegSize(otherReg.reg, bits);

                    if (mainRegSize != otherRegSize)
                        throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                    modrm.reg = otherI;
                    modrm.rm = mainI;

                    if (mainRegSize == 8)
                    {
                        switch (mnemonic)
                        {
                            case ADD: opcode = 0x00; break;
                            case OR:  opcode = 0x08; break;
                            case ADC: opcode = 0x10; break;
                            case SBB: opcode = 0x18; break;
                            case AND: opcode = 0x20; break;
                            case SUB: opcode = 0x28; break;
                            case XOR: opcode = 0x30; break;
                            case CMP: opcode = 0x38; break;
                            case TEST: opcode = 0x84; break;
                        }
                    }
                    else
                    {
                        switch (mnemonic)
                        {
                            case ADD: opcode = 0x01; break;
                            case OR:  opcode = 0x09; break;
                            case ADC: opcode = 0x11; break;
                            case SBB: opcode = 0x19; break;
                            case AND: opcode = 0x21; break;
                            case SUB: opcode = 0x29; break;
                            case XOR: opcode = 0x31; break;
                            case CMP: opcode = 0x39; break;
                            case TEST: opcode = 0x85; break;
                        }
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

                    if (rex.use && (mainReg.reg == AH || mainReg.reg == CH ||
                                    mainReg.reg == DH || mainReg.reg == BH ||
                                    otherReg.reg == AH || otherReg.reg == CH ||
                                    otherReg.reg == DH || otherReg.reg == BH))
                        throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(otherOperand))
                {
                    throw Exception::InternalError("Memory not supported yet'", -1, -1);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(otherOperand))
                {
                    aluType = AluType::ALU_REG_IMM;

                    bool is64bit = false;

                    // TODO
                    switch (mainReg.reg)
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
                            specific.alu_reg_imm.max = std::numeric_limits<uint8_t>::max();
                            specific.alu_reg_imm.sizeInBits = 8;
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
                            specific.alu_reg_imm.max = std::numeric_limits<uint16_t>::max();
                            specific.alu_reg_imm.sizeInBits = 16;

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
                            specific.alu_reg_imm.max = std::numeric_limits<uint32_t>::max();
                            specific.alu_reg_imm.sizeInBits = 32;

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
                            specific.alu_reg_imm.max = std::numeric_limits<uint32_t>::max();
                            specific.alu_reg_imm.sizeInBits = 32;

                            is64bit = true;

                            rex.use = true;
                            rex.w = true;

                            break;

                        default:
                            throw Exception::SemanticError("instruction doesn't support this register", -1, -1);
                    }

                    bool accumulatorReg = false;

                    switch (mainReg.reg)
                    {
                        case AL:
                            accumulatorReg = true;
                            break;
                        
                        case AX:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;
                            
                            accumulatorReg = true;
                            break;
                        
                        case EAX:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;

                            accumulatorReg = true;
                            break;

                        case RAX:
                            rex.use = true;
                            rex.w = true;

                            accumulatorReg = true;
                            break;
                    }

                    if (accumulatorReg)
                    {
                        if (specific.alu_reg_imm.sizeInBits == 8)
                        {
                            switch (mnemonic)
                            {
                                case ADD: opcode = 0x04; break;
                                case OR:  opcode = 0x0C; break;
                                case ADC: opcode = 0x14; break;
                                case SBB: opcode = 0x1C; break;
                                case AND: opcode = 0x24; break;
                                case SUB: opcode = 0x2C; break;
                                case XOR: opcode = 0x34; break;
                                case CMP: opcode = 0x3C; break;
                                case TEST: opcode = 0xA8; break;
                            }
                        }
                        else
                        {
                            switch (mnemonic)
                            {
                                case ADD: opcode = 0x05; break;
                                case OR:  opcode = 0x0D; break;
                                case ADC: opcode = 0x15; break;
                                case SBB: opcode = 0x1D; break;
                                case AND: opcode = 0x25; break;
                                case SUB: opcode = 0x2D; break;
                                case XOR: opcode = 0x35; break;
                                case CMP: opcode = 0x3D; break;
                                case TEST: opcode = 0xA9; break;
                            }
                        }
                    }
                    else
                    {
                        auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);

                        if (mainUseREX) rex.use = true;
                        if (mainSetREX) rex.b = true;

                        modrm.use = true;

                        modrm.mod = Mod::REGISTER;
                        modrm.rm = mainI;

                        if (mnemonic == TEST)
                        {
                            modrm.reg = 0;
                            
                            if (specific.alu_reg_imm.sizeInBits == 8)
                            {
                                opcode = 0xF6;
                            }
                            else
                            {
                                opcode = 0xF7;
                            }
                        }
                        else
                        {
                            switch (mnemonic)
                            {
                                case ADD: modrm.reg = 0; break;
                                case OR:  modrm.reg = 1; break;
                                case ADC: modrm.reg = 2; break;
                                case SBB: modrm.reg = 3; break;
                                case AND: modrm.reg = 4; break;
                                case SUB: modrm.reg = 5; break;
                                case XOR: modrm.reg = 6; break;
                                case CMP: modrm.reg = 7; break;
                            }

                            if (specific.alu_reg_imm.sizeInBits == 8)
                            {
                                opcode = 0x80;
                            }
                            else
                            {
                                opcode = 0x81;
                            }
                        }
                    }
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                // TODO
                throw Exception::InternalError("Memory not supported yet", -1, -1);

                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(otherOperand))
                {
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(otherOperand))
                {
                    // TODO
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown two argument alu instruction", -1, -1, nullptr);
    }
}

void x86::Two_Argument_ALU_Instruction::evaluate()
{
    switch (aluType)
    {
        case AluType::ALU_REG_REG: break;

        case AluType::ALU_REG_IMM:
        {
            Parser::Immediate imm = std::get<Parser::Immediate>(otherOperand);

            ::Encoder::Evaluation evaluation = Evaluate(imm);

            if (evaluation.useOffset)
            {
                usedReloc = true;
                evalUsedSection = evaluation.usedSection;
                evalIsExtern = evaluation.isExtern;
                specific.alu_reg_imm.value = evaluation.offset;
            }
            else
            {
                Int128 result = evaluation.result;

                // FIXME
                if (result > specific.alu_reg_imm.max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

                specific.alu_reg_imm.value = static_cast<uint64_t>(result);
            }

            break;
        }
    }
}

bool x86::Two_Argument_ALU_Instruction::optimize()
{
    return false;
}

void x86::Two_Argument_ALU_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    switch (aluType)
    {
        case AluType::ALU_REG_REG:
        {
            // TODO
            break;
        }

        case AluType::ALU_REG_IMM:
        {
            uint32_t sizeInBytes = specific.alu_reg_imm.sizeInBits / 8;

            uint64_t oldSize = buffer.size();
            buffer.resize(oldSize + sizeInBytes);

            std::memcpy(buffer.data() + oldSize, &specific.alu_reg_imm.value, sizeInBytes);

            if (usedReloc)
            {
                uint64_t currentOffset = 1; // opcode
                if (use16BitPrefix) currentOffset++;
                if (rex.use) currentOffset++;
                if (modrm.use) currentOffset++;

                ::Encoder::RelocationSize relocSize;
                switch (specific.alu_reg_imm.sizeInBits)
                {
                    case 8: relocSize = ::Encoder::RelocationSize::Bit8; break;
                    case 16: relocSize = ::Encoder::RelocationSize::Bit16; break;
                    case 32: relocSize = ::Encoder::RelocationSize::Bit32; break;
                    case 64: relocSize = ::Encoder::RelocationSize::Bit64; break;
                    default: throw Exception::InternalError("Unknown size in bits " + std::to_string(specific.alu_reg_imm.sizeInBits), -1, -1);
                }

                AddRelocation(
                    currentOffset,
                    specific.alu_reg_imm.value,
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

uint64_t x86::Two_Argument_ALU_Instruction::sizeS()
{
    uint64_t s = 0;

    switch (aluType)
    {
        case AluType::ALU_REG_REG:
        {
            // TODO
            break;
        }

        case AluType::ALU_REG_IMM:
        {
            uint32_t sizeInBytes = specific.alu_reg_imm.sizeInBits / 8;

            s += sizeInBytes;

            break;
        }

        default:
            throw Exception::InternalError("Unknown aluType", -1, -1);
    }

    return s;
}
