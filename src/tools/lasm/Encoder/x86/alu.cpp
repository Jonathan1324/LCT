#include "alu.hpp"

#include <x86/Instructions.hpp>
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

std::vector<uint8_t> x86::Two_Argument_ALU_Instruction::encode()
{
    std::vector<uint8_t> instr;

    if (use16BitPrefix) instr.push_back(prefix16Bit);

    if (rex.use) instr.push_back(::x86::getRex(rex.w, rex.r, rex.x, rex.b));

    instr.push_back(opcode);

    if (modrm.use) instr.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));

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

            uint64_t oldSize = instr.size();
            instr.resize(oldSize + sizeInBytes);

            std::memcpy(instr.data() + oldSize, &specific.alu_reg_imm.value, sizeInBytes);

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

    return instr;
}

uint64_t x86::Two_Argument_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;

    if (rex.use) s++;

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

    if (modrm.use) s++;

    return s;
}

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

std::vector<uint8_t> x86::Mul_Div_ALU_Instruction::encode()
{
    std::vector<uint8_t> instr;

    if (use16BitPrefix) instr.push_back(prefix16Bit);

    if (rex.use) instr.push_back(::x86::getRex(rex.w, rex.r, rex.x, rex.b));

    if (useOpcodeEscape) instr.push_back(opcodeEscape);

    instr.push_back(opcode);

    if (modrm.use) instr.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));

    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            uint32_t sizeInBytes = threeOperandsSpecific.sizeInBits / 8;

            uint64_t oldSize = instr.size();
            instr.resize(oldSize + sizeInBytes);

            std::memcpy(instr.data() + oldSize, &threeOperandsSpecific.value, sizeInBytes);

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

    return instr;
}

uint64_t x86::Mul_Div_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;

    if (rex.use) s++;

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

void x86::Shift_Rotate_ALU_Instruction::evaluate()
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

std::vector<uint8_t> x86::Shift_Rotate_ALU_Instruction::encode()
{
    std::vector<uint8_t> instr;

    if (use16BitPrefix) instr.push_back(prefix16Bit);

    if (rex.use) instr.push_back(::x86::getRex(rex.w, rex.r, rex.x, rex.b));

    instr.push_back(opcode);

    if (modrm.use) instr.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));

    if (usesImmediate)
    {
        instr.push_back(count);

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

    return instr;
}

uint64_t x86::Shift_Rotate_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;

    if (rex.use) s++;

    if (modrm.use) s++;

    if (usesImmediate) s++;

    return s;
}

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

            if (std::holds_alternative<Parser::Instruction::Register>(operand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);

                switch (reg.reg)
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

                modrm.use = true;

                modrm.mod = Mod::REGISTER;

                auto [idx, regUseREX, regSetREX] = getReg(reg.reg);
                uint8_t regSize = getRegSize(reg.reg, bits);

                if (regUseREX) rex.use = true;
                if (regSetREX) rex.b = true;

                modrm.rm = idx;

                if (regSize == 8)
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

                switch (regSize)
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

                if (bits != BitMode::Bits64 && regSize != 8 &&
                    (mnemonic == Instructions::INC || mnemonic == Instructions::DEC))
                {
                    modrm.use = false;

                    if (mnemonic == Instructions::INC)
                        opcode = 0x40 + idx;
                    else // DEC
                        opcode = 0x48 + idx;
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                
                modrm.use = true;

                modrm.mod = Mod::INDIRECT;

                if (mem.pointer_size == Parser::Instruction::Memory::NO_POINTER_SIZE)
                    throw Exception::SyntaxError("Pointer size not specified for memory operand", -1, -1);

                uint8_t mem_reg_size;

                if (mem.use_reg1 && mem.use_reg2)
                {
                    uint8_t base_reg_size = getRegSize(mem.reg1, bits);
                    uint8_t index_reg_size = getRegSize(mem.reg2, bits);
                    if (base_reg_size != index_reg_size)
                        throw Exception::SyntaxError("Base register and index registers have different sizes", -1, -1);
                    mem_reg_size = base_reg_size;
                }
                else if (mem.use_reg1 && !mem.use_reg2)
                {
                    mem_reg_size = getRegSize(mem.reg1, bits);
                }
                else if (!mem.use_reg1 && mem.use_reg2)
                {
                    mem_reg_size = getRegSize(mem.reg2, bits);
                }
                else
                {
                    if (bits == BitMode::Bits16)      mem_reg_size = 16;
                    else if (bits == BitMode::Bits32) mem_reg_size = 32;
                    else                              mem_reg_size = 64;
                }

                bool hasBase = false;
                uint64_t baseReg;

                bool hasIndex = false;
                uint64_t indexReg;

                uint64_t scale;

                if      (mem_reg_size == 16) use16BitAddressing = true;
                else if (mem_reg_size == 64) use64BitAddressing = true;

                // FIXME: Fix
                if (mem.use_reg1 && mem.use_reg2)
                {
                    ::Encoder::Evaluation reg1_evaluation = Evaluate(mem.scale1);
                    ::Encoder::Evaluation reg2_evaluation = Evaluate(mem.scale2);

                    if (reg1_evaluation.useOffset || reg2_evaluation.useOffset)
                        throw Exception::SemanticError("Scale must be a constant", -1, -1); // TODO

                    Int128& reg1_result = reg1_evaluation.result;
                    Int128& reg2_result = reg2_evaluation.result;

                    hasBase = true;
                    hasIndex = true;

                    if (reg1_result != 1 && reg2_result != 1)
                        throw Exception::SemanticError("Can't have 2 index registers", -1, -1);

                    if (reg1_result == 1 && reg2_result == 1)
                    {
                        if (mem.reg1 == Registers::ESP || mem.reg1 == Registers::RSP)
                        {
                            baseReg = mem.reg1;

                            indexReg = mem.reg2;
                            scale = static_cast<uint64_t>(reg2_result);
                        }
                        else if (mem.reg2 == Registers::ESP || mem.reg2 == Registers::RSP)
                        {
                            baseReg = mem.reg2;

                            indexReg = mem.reg1;
                            scale = static_cast<uint64_t>(reg1_result);
                        }
                        else if (mem.reg1 == Registers::BX || mem.reg1 == Registers::BP)
                        {
                            baseReg = mem.reg1;

                            indexReg = mem.reg2;
                            scale = static_cast<uint64_t>(reg2_result);
                        }
                        else if (mem.reg2 == Registers::BX || mem.reg2 == Registers::BP)
                        {
                            baseReg = mem.reg2;

                            indexReg = mem.reg1;
                            scale = static_cast<uint64_t>(reg1_result);
                        }
                        else
                        {
                            baseReg = mem.reg1;

                            indexReg = mem.reg2;
                            scale = static_cast<uint64_t>(reg2_result);
                        }
                    }
                    else if (reg1_result == 1 && reg2_result != 1)
                    {
                        baseReg = mem.reg1;

                        indexReg = mem.reg2;
                        scale = static_cast<uint64_t>(reg2_result);
                    }
                    else // if (reg1_result != 1 && reg2_result == 1)
                    {
                        baseReg = mem.reg2;

                        indexReg = mem.reg1;
                        scale = static_cast<uint64_t>(reg1_result);
                    }
                }
                else if ((mem.use_reg1 && !mem.use_reg2) || (mem.use_reg2 && !mem.use_reg1))
                {
                    uint64_t usedReg;
                    const Parser::Immediate* usedScale;

                    if (mem.use_reg1 && !mem.use_reg2)
                    {
                        usedReg = mem.reg1;
                        usedScale = &mem.scale1;
                    }
                    else // (mem.use_reg2 && !mem.use_reg1)
                    {
                        usedReg = mem.reg2;
                        usedScale = &mem.scale2;
                    }

                    ::Encoder::Evaluation evaluation = Evaluate(*usedScale);

                    if (evaluation.useOffset)
                        throw Exception::SemanticError("Scale must be a constant", -1, -1); // TODO

                    Int128& result = evaluation.result;

                    if (use16BitAddressing)
                    {
                        scale = result;
                        
                        if (usedReg == Registers::BX || usedReg == Registers::BP)
                        {
                            hasBase = true;
                            baseReg = usedReg;
                        }
                        else
                        {
                            hasIndex = true;
                            indexReg = usedReg;
                        }
                    }
                    else
                    {
                        switch (result)
                        {
                            case 1:
                                hasBase = true;
                                baseReg = usedReg;
                                break;
                            
                            case 2:
                                hasBase = true;
                                baseReg = usedReg;

                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 1;
                                break;

                            case 3:
                                hasBase = true;
                                baseReg = usedReg;

                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 2;
                                break;

                            case 4:
                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 4;
                                break;

                            case 5:
                                hasBase = true;
                                baseReg = usedReg;

                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 4;
                                break;

                            case 8:
                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 8;
                                break;

                            case 9:
                                hasBase = true;
                                baseReg = usedReg;

                                hasIndex = true;
                                indexReg = usedReg;
                                scale = 8;
                                break;

                            case 6: case 7: default:
                                throw Exception::SemanticError("Scale needs to be 1, 2, 4 or 8", -1, -1);
                        }
                    }
                }

                if (hasIndex)
                {
                    if (use16BitAddressing)
                    {
                        if (scale != 1)
                            throw Exception::SemanticError("Scale must be 1 when using 16 bit addressing", -1, -1);
                    }
                    else
                    {
                        switch (scale)
                        {
                            case 1: sib.scale = Scale::x1; break;
                            case 2: sib.scale = Scale::x2; break;
                            case 4: sib.scale = Scale::x4; break;
                            case 8: sib.scale = Scale::x8; break;
                            default:
                                throw Exception::SemanticError("Scale must be 1, 2, 4, or 8", -1, -1); // TODO
                        }
                    }
                }

                if (mem_reg_size == 16)
                {
                    if (bits == BitMode::Bits64)
                        throw Exception::SemanticError("Can't use 16 bit registers in 64 bit mode", -1, -1);
                    else if (bits == BitMode::Bits32)
                        use16BitAddressPrefix = true;

                    if (hasBase && (baseReg != Registers::BX && baseReg != Registers::BP))
                        throw Exception::SemanticError("Only bx/bp allowed as 16-bit base", -1, -1);
                    
                    if (hasIndex && (indexReg != Registers::SI && indexReg != Registers::DI))
                        throw Exception::SemanticError("Only si/di allowed as 16-bit index", -1, -1);

                    uint8_t addressing_mode;
                    if (hasBase && hasIndex)
                    {
                        // [bx+si], [bx+di], [bp+si], [bp+di]
                        if (baseReg == Registers::BX && indexReg == Registers::SI) addressing_mode = 0;  // 000b
                        else if (baseReg == Registers::BX && indexReg == Registers::DI) addressing_mode = 1;  // 001b
                        else if (baseReg == Registers::BP && indexReg == Registers::SI) addressing_mode = 2;  // 010b
                        else if (baseReg == Registers::BP && indexReg == Registers::DI) addressing_mode = 3;  // 011b
                        else
                            throw Exception::SemanticError("Invalid 16-bit addressing combination", -1, -1);
                    }
                    else if (hasBase && !hasIndex)
                    {
                        // [bx], [bp]
                        if (baseReg == Registers::BX) addressing_mode = 7;  // 111b
                        else if (baseReg == Registers::BP) addressing_mode = 6;  // 110b
                        else
                            throw Exception::SemanticError("Invalid 16-bit base register", -1, -1);
                    }
                    else if (!hasBase && hasIndex)
                    {
                        // [si], [di]
                        if (indexReg == Registers::SI) addressing_mode = 4;  // 100b
                        else if (indexReg == Registers::DI) addressing_mode = 5;  // 101b
                        else
                            throw Exception::SemanticError("Invalid 16-bit index register", -1, -1);
                    }
                    else
                    {
                        addressing_mode = 6;  // Direct address
                    }

                    modrm.rm = addressing_mode;

                    if (mem.use_displacement)
                    {
                        use_displacement = true;
                        displacement_immediate = mem.displacement;

                        if (addressing_mode == 6)  // Direct address
                            modrm.mod = Mod::INDIRECT;
                        else
                            modrm.mod = Mod::INDIRECT_DISP32;
                    }
                    else
                    {
                        modrm.mod = Mod::INDIRECT;
                    }
                }
                else // 32,64
                {
                    if (mem_reg_size == 32 && bits != BitMode::Bits32)
                        use16BitAddressPrefix = true;

                    if (mem_reg_size == 64)
                    {
                        if (bits != BitMode::Bits64)
                            throw Exception::SemanticError("64-bit registers only in 64-bit mode", -1, -1);
                        is_displacement_signed = true;
                    }

                    if (hasBase)
                    {
                        auto [baseI, baseUseREX, baseSetREX] = getReg(baseReg);

                        if (baseUseREX) rex.use = true;
                        if (baseSetREX) rex.b = true;

                        modrm.rm = baseI;

                        sib.index = SIB_NoIndex;
                        if (baseI == modRMSIB) sib.use = true;

                        if (baseI == modRMDisp)
                        {
                            modrm.mod = Mod::INDIRECT_DISP8;
                        }
                    }
                    else if (hasIndex)
                    {
                        modrm.mod = Mod::INDIRECT;
                        modrm.rm = modRMDisp;
                        
                        sib.use = true;
                        sib.index = SIB_NoIndex;
                    }

                    if (hasIndex)
                    {
                        auto [indexI, indexUseREX, indexSetREX] = getReg(indexReg);

                        if (indexI == SIB_NoIndex && !indexSetREX)
                            throw Exception::SemanticError("Can't use ESP/RSP as index register", -1, -1);

                        if (indexUseREX) rex.use = true;
                        if (indexSetREX) rex.x = true;

                        sib.use = true;
                        sib.index = indexI;
                    }

                    if (mem.use_displacement)
                    {
                        if (hasBase) modrm.mod = Mod::INDIRECT_DISP32;

                        use_displacement = true;
                        displacement_immediate = mem.displacement;

                        if (!hasBase && !hasIndex)
                        {
                            if (bits == BitMode::Bits64)
                            {
                                modrm.mod = Mod::INDIRECT;
                                modrm.rm = modRMDisp;
                            
                                sib.use = true;
                                sib.scale = Scale::x1;
                                sib.index = SIB_NoIndex;
                            }
                            else
                            {
                                modrm.mod = Mod::INDIRECT;
                                modrm.rm = modRMDisp;
                            }
                        }
                    }
                }

                if (mem.pointer_size == 8)
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

                switch (mem.pointer_size)
                {
                    case 16:
                        if (bits != BitMode::Bits16) use16BitPrefix = true;
                        break;
                    
                    case 32:
                        if (bits == BitMode::Bits16) use16BitPrefix = true;
                        break;

                    case 64:
                        if (bits != BitMode::Bits64)
                                throw Exception::SemanticError("Can't use 64 bit size of memory in 16/32 bit", -1, -1);
                        rex.use = true;
                        rex.w = true;
                        break;

                    case 8:
                        break;

                    default:
                        throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument ALU instruction", -1, -1, nullptr);
    }
}

void x86::Argument_ALU_Instruction::evaluate()
{
    if (use_displacement)
    {
        ::Encoder::Evaluation evaluation = Evaluate(displacement_immediate);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            evalUsedSection = evaluation.usedSection;
            evalIsExtern = evaluation.isExtern;
            displacement_value = evaluation.offset;
            displacement_can_optimize = false;
        }
        else
        {
            Int128 result = evaluation.result;

            // TODO: Check for overflow

            displacement_value = static_cast<uint64_t>(result);
        }
    }
}

std::vector<uint8_t> x86::Argument_ALU_Instruction::encode()
{
    std::vector<uint8_t> instr;

    if (use16BitPrefix) instr.push_back(prefix16Bit);
    if (use16BitAddressPrefix) instr.push_back(addressPrefix16Bit);

    if (rex.use) instr.push_back(::x86::getRex(rex.w, rex.r, rex.x, rex.b));

    instr.push_back(opcode);

    if (modrm.use)
    {
        if (sib.use) instr.push_back(getModRM(modrm.mod, modrm.reg, modRMSIB));
        else        instr.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));
    }

    if (sib.use) instr.push_back(getSIB(sib.scale, sib.index, modrm.rm));

    bool directAddressing = false;
    if (use16BitAddressing) directAddressing = (modrm.mod == Mod::INDIRECT && modrm.rm == 6);
    else                    directAddressing = (modrm.mod == Mod::INDIRECT && modrm.rm == modRMDisp);

    // TODO: FIXME: EVERYTHING
    if (modrm.mod == Mod::INDIRECT_DISP8)
    {
        instr.push_back(static_cast<uint8_t>(displacement_value));

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (use16BitAddressPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (modrm.use) currentOffset++;
            if (sib.use) currentOffset++;

            AddRelocation(
                currentOffset,
                displacement_value,
                true,
                evalUsedSection,
                ::Encoder::RelocationType::Absolute,
                ::Encoder::RelocationSize::Bit8,
                is_displacement_signed,
                evalIsExtern
            );
        }
    }
    else if (modrm.mod == Mod::INDIRECT_DISP32 || directAddressing)
    {
        uint32_t sizeInBytes;
        if (use16BitAddressing) sizeInBytes = 2;
        else                    sizeInBytes = 4;

        uint64_t oldSize = instr.size();
        instr.resize(oldSize + sizeInBytes);

        std::memcpy(instr.data() + oldSize, &displacement_value, sizeInBytes);

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (use16BitAddressPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (modrm.use) currentOffset++;
            if (sib.use) currentOffset++;

            ::Encoder::RelocationSize relocSize;
            if (use16BitAddressing) relocSize = ::Encoder::RelocationSize::Bit16;
            else                    relocSize = ::Encoder::RelocationSize::Bit32;

            AddRelocation(
                currentOffset,
                displacement_value,
                true,
                evalUsedSection,
                ::Encoder::RelocationType::Absolute,
                relocSize,
                is_displacement_signed,
                evalIsExtern
            );
        }
    }

    return instr;
}

uint64_t x86::Argument_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;
    if (use16BitAddressPrefix) s++;

    if (rex.use) s++;

    if (modrm.use) s++;

    if (sib.use) s++;

    bool directAddressing = false;
    if (use16BitAddressing) directAddressing = (modrm.mod == Mod::INDIRECT && modrm.rm == 6);
    else                    directAddressing = (modrm.mod == Mod::INDIRECT && modrm.rm == modRMDisp);

    // TODO: FIXME: EVERYTHING
    if      (modrm.mod == Mod::INDIRECT_DISP8)  s++;
    else if (modrm.mod == Mod::INDIRECT_DISP32 || directAddressing)
    {
        if (use16BitAddressing) s += 2;
        else                    s += 4;
    }

    return s;
}

bool x86::Argument_ALU_Instruction::optimize()
{
    if (use_displacement && modrm.mod != Mod::INDIRECT && displacement_can_optimize && !displacement_optimized)
    {
        if (displacement_value < -128 || displacement_value > 127)
            return false;

        displacement_optimized = true;

        modrm.mod = Mod::INDIRECT_DISP8;

        return true;
    }
    
    return false;
}
