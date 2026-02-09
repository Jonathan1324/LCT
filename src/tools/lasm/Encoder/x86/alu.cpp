#include "alu.hpp"

#include <x86/Instructions.hpp>
#include <limits>
#include <cstring>

x86::Two_Argument_ALU_Instruction::Two_Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::Encoder::Encoder::Instruction(e)
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

                    mod_mod = ::x86::Mod::REGISTER;

                    useModRM = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    auto [otherI, otherUseREX, otherSetREX] = ::x86::getReg(otherReg.reg);

                    if (mainUseREX || otherUseREX)
                        useREX = true;
                    if (mainSetREX)
                        rexB = true;
                    if (otherSetREX)
                        rexR = true;

                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);
                    uint8_t otherRegSize = getRegSize(otherReg.reg, bits);

                    if (mainRegSize != otherRegSize)
                        throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                    mod_reg = otherI;
                    mod_rm = mainI;

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
                            useREX = true;
                            rexW = true;
                            break;

                        case 8:
                            break;

                        default:
                            throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                    }

                    if (useREX && (mainReg.reg == AH || mainReg.reg == CH ||
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

                            useREX = true;
                            rexW = true;

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
                            useREX = true;
                            rexW = true;

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

                        if (mainUseREX) useREX = true;
                        if (mainSetREX) rexB = true;

                        useModRM = true;

                        mod_mod = Mod::REGISTER;
                        mod_rm = mainI;

                        if (mnemonic == TEST)
                        {
                            mod_reg = 0;
                            
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
                                case ADD: mod_reg = 0; break;
                                case OR:  mod_reg = 1; break;
                                case ADC: mod_reg = 2; break;
                                case SBB: mod_reg = 3; break;
                                case AND: mod_reg = 4; break;
                                case SUB: mod_reg = 5; break;
                                case XOR: mod_reg = 6; break;
                                case CMP: mod_reg = 7; break;
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
                uint64_t currentOffset = 1; // opcode
                if (use16BitPrefix) currentOffset++;
                if (useREX) currentOffset++;
                if (useModRM) currentOffset++;

                specific.alu_reg_imm.value = evaluation.offset; // TODO: Check for overflow

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
                    evaluation.offset,
                    true,
                    evaluation.usedSection,
                    ::Encoder::RelocationType::Absolute,
                    relocSize,
                    false, // TODO: Check if signed
                    evaluation.isExtern
                );
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

    if (useREX) instr.push_back(::x86::getRex(rexW, rexR, rexX, rexB));

    instr.push_back(opcode);

    if (useModRM) instr.push_back(getModRM(mod_mod, mod_reg, mod_rm));

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

    if (useREX) s++;

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

    if (useModRM) s++;

    return s;
}

x86::Mul_Div_ALU_Instruction::Mul_Div_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::Encoder::Encoder::Instruction(e)
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

                if (mainUseREX) useREX = true;
                if (mainSetREX) rexR = true;

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

                        mod_mod = Mod::REGISTER;

                        useModRM = true;

                        auto [secondI, secondUseREX, secondSetREX] = ::x86::getReg(secondReg.reg);
                        uint8_t secondRegSize = getRegSize(secondReg.reg, bits);

                        if (secondUseREX) useREX = true;
                        if (secondSetREX) rexB = true;

                        if (mainRegSize != secondRegSize)
                            throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);

                        mod_reg = mainI;
                        mod_rm = secondI;

                        switch (mainRegSize)
                        {
                            case 16:
                                if (bits != BitMode::Bits16) use16BitPrefix = true;
                                break;
                            
                            case 32:
                                if (bits == BitMode::Bits16) use16BitPrefix = true;
                                break;

                            case 64:
                                useREX = true;
                                rexW = true;
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

                        mod_mod = Mod::REGISTER;

                        useModRM = true;

                        auto [secondI, secondUseREX, secondSetREX] = ::x86::getReg(secondReg.reg);
                        uint8_t secondRegSize = getRegSize(secondReg.reg, bits);

                        if (secondUseREX) useREX = true;
                        if (secondSetREX) rexB = true;

                        if (mainRegSize != secondRegSize)
                            throw Exception::SemanticError("Can't use instruction with registers of different size", -1, -1);
                        
                        mod_reg = mainI;
                        mod_rm = secondI;

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

                                useREX = true;
                                rexW = true;
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

                mod_mod = Mod::REGISTER;

                useModRM = true;

                auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);

                mod_rm = mainI;

                if (mainUseREX) useREX = true;
                if (mainSetREX) rexB = true;

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
                        useREX = true;
                        rexW = true;
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
                    case MUL:  mod_reg = 4; break;
                    case IMUL: mod_reg = 5; break;
                    case DIV:  mod_reg = 6; break;
                    case IDIV: mod_reg = 7; break;
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
                uint64_t currentOffset = 1; // Opcode
                if (use16BitPrefix) currentOffset++;
                if (useREX) currentOffset++;
                if (useOpcodeEscape) currentOffset++;
                if (useModRM) currentOffset++;

                threeOperandsSpecific.value = evaluation.offset; // TODO: Check for overflow

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
                    evaluation.offset,
                    true,
                    evaluation.usedSection,
                    ::Encoder::RelocationType::Absolute,
                    relocSize,
                    false, // TODO: Check if signed
                    evaluation.isExtern
                );
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

    if (useREX) instr.push_back(::x86::getRex(rexW, rexR, rexX, rexB));

    if (useOpcodeEscape) instr.push_back(opcodeEscape);

    instr.push_back(opcode);

    if (useModRM) instr.push_back(getModRM(mod_mod, mod_reg, mod_rm));

    switch (mulDivType)
    {
        case MulDivType::Simple: case MulDivType::TwoOperands: break;

        case MulDivType::ThreeOperands:
        {
            uint32_t sizeInBytes = threeOperandsSpecific.sizeInBits / 8;

            uint64_t oldSize = instr.size();
            instr.resize(oldSize + sizeInBytes);

            std::memcpy(instr.data() + oldSize, &threeOperandsSpecific.value, sizeInBytes);

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

    if (useREX) s++;

    if (useOpcodeEscape) s++;

    if (useModRM) s++;

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
    : ::Encoder::Encoder::Instruction(e)
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

                    mod_mod = Mod::REGISTER;

                    useModRM = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

                    if (mainUseREX) useREX = true;
                    if (mainSetREX) rexB = true;

                    mod_rm = mainI;

                    if (mainRegSize == 8) opcode = 0xD2;
                    else                  opcode = 0xD3;

                    switch (mnemonic)
                    {
                        case Instructions::ROL: mod_reg = 0; break;
                        case Instructions::ROR: mod_reg = 1; break;
                        case Instructions::RCL: mod_reg = 2; break;
                        case Instructions::RCR: mod_reg = 3; break;
                        case Instructions::SHL: case Instructions::SAL:
                            mod_reg = 4; break;
                        case Instructions::SHR: mod_reg = 5; break;
                        case Instructions::SAR: mod_reg = 7; break;
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
                            useREX = true;
                            rexW = true;
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

                    mod_mod = Mod::REGISTER;

                    useModRM = true;

                    auto [mainI, mainUseREX, mainSetREX] = ::x86::getReg(mainReg.reg);
                    uint8_t mainRegSize = getRegSize(mainReg.reg, bits);

                    if (mainUseREX) useREX = true;
                    if (mainSetREX) rexB = true;

                    mod_rm = mainI;

                    if (mainRegSize == 8) opcode = 0xC0;
                    else                  opcode = 0xC1;

                    switch (mnemonic)
                    {
                        case Instructions::ROL: mod_reg = 0; break;
                        case Instructions::ROR: mod_reg = 1; break;
                        case Instructions::RCL: mod_reg = 2; break;
                        case Instructions::RCR: mod_reg = 3; break;
                        case Instructions::SHL: case Instructions::SAL:
                            mod_reg = 4; break;
                        case Instructions::SHR: mod_reg = 5; break;
                        case Instructions::SAR: mod_reg = 7; break;
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
                            useREX = true;
                            rexW = true;
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
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (useREX) currentOffset++;
            if (useModRM) currentOffset++;

            count = static_cast<uint8_t>(evaluation.offset); // TODO: Check for overflow

            AddRelocation(
                currentOffset,
                evaluation.offset,
                true,
                evaluation.usedSection,
                ::Encoder::RelocationType::Absolute,
                ::Encoder::RelocationSize::Bit8,
                false, // TODO: Check if signed
                evaluation.isExtern
            );
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

    if (useREX) instr.push_back(::x86::getRex(rexW, rexR, rexX, rexB));

    instr.push_back(opcode);

    if (useModRM) instr.push_back(getModRM(mod_mod, mod_reg, mod_rm));

    if (usesImmediate)
    {
        instr.push_back(count);
    }

    return instr;
}

uint64_t x86::Shift_Rotate_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;

    if (useREX) s++;

    if (useModRM) s++;

    if (usesImmediate) s++;

    return s;
}

x86::Argument_ALU_Instruction::Argument_ALU_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::Encoder::Encoder::Instruction(e)
{
    bitmode = bits;
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

                useModRM = true;

                mod_mod = Mod::REGISTER;

                auto [idx, regUseREX, regSetREX] = getReg(reg.reg);
                uint8_t regSize = getRegSize(reg.reg, bits);

                if (regUseREX) useREX = true;
                if (regSetREX) rexB = true;

                mod_rm = idx;

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
                    case Instructions::NOT: mod_reg = 2; break;
                    case Instructions::NEG: mod_reg = 3; break;

                    case Instructions::INC: mod_reg = 0; break;
                    case Instructions::DEC: mod_reg = 1; break;
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
                        useREX = true;
                        rexW = true;
                        break;

                    case 8:
                        break;

                    default:
                        throw Exception::InternalError("Unknown mainRegSize", -1, -1);
                }

                if (bits != BitMode::Bits64 && regSize != 8 &&
                    (mnemonic == Instructions::INC || mnemonic == Instructions::DEC))
                {
                    useModRM = false;

                    if (mnemonic == Instructions::INC)
                        opcode = 0x40 + idx;
                    else // DEC
                        opcode = 0x48 + idx;
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                
                useModRM = true;

                mod_mod = Mod::INDIRECT;

                if (mem.pointer_size == Parser::Instruction::Memory::NO_POINTER_SIZE)
                    throw Exception::SyntaxError("Pointer size not specified for memory operand", -1, -1);

                uint8_t mem_reg_size;

                if (mem.use_base_reg && mem.use_index_reg)
                {
                    uint8_t base_reg_size = getRegSize(mem.base_reg, bits);
                    uint8_t index_reg_size = getRegSize(mem.index_reg, bits);
                    if (base_reg_size != index_reg_size)
                        throw Exception::SyntaxError("Base register and index registers have different sizes", -1, -1);
                    mem_reg_size = base_reg_size;
                }
                else if (mem.use_base_reg && !mem.use_index_reg)
                {
                    mem_reg_size = getRegSize(mem.base_reg, bits);
                }
                else if (!mem.use_base_reg && mem.use_index_reg)
                {
                    mem_reg_size = getRegSize(mem.index_reg, bits);
                }
                else
                {
                    if (bits == BitMode::Bits16)      mem_reg_size = 16;
                    else if (bits == BitMode::Bits32) mem_reg_size = 32;
                    else                              mem_reg_size = 64;
                }

                if (mem_reg_size == 16)
                {
                    use16BitAddressing = true;

                    if (bits == BitMode::Bits64)
                        throw Exception::SemanticError("Can't use 16 bit registers in 64 bit mode", -1, -1);
                    else if (bits == BitMode::Bits32)
                        use16BitAddressPrefix = true;

                    if (mem.use_base_reg && (mem.base_reg != Registers::BX && mem.base_reg != Registers::BP))
                        throw Exception::SemanticError("Only bx/bp allowed as 16-bit base", -1, -1);
                    
                    if (mem.use_index_reg && (mem.index_reg != Registers::SI && mem.index_reg != Registers::DI))
                        throw Exception::SemanticError("Only si/di allowed as 16-bit index", -1, -1);

                    uint8_t addressing_mode;
                    if (mem.use_base_reg && mem.use_index_reg)
                    {
                        // [bx+si], [bx+di], [bp+si], [bp+di]
                        if (mem.base_reg == Registers::BX && mem.index_reg == Registers::SI) addressing_mode = 0;  // 000b
                        else if (mem.base_reg == Registers::BX && mem.index_reg == Registers::DI) addressing_mode = 1;  // 001b
                        else if (mem.base_reg == Registers::BP && mem.index_reg == Registers::SI) addressing_mode = 2;  // 010b
                        else if (mem.base_reg == Registers::BP && mem.index_reg == Registers::DI) addressing_mode = 3;  // 011b
                        else
                            throw Exception::SemanticError("Invalid 16-bit addressing combination", -1, -1);
                    }
                    else if (mem.use_base_reg && !mem.use_index_reg)
                    {
                        // [bx], [bp]
                        if (mem.base_reg == Registers::BX) addressing_mode = 7;  // 111b
                        else if (mem.base_reg == Registers::BP) addressing_mode = 6;  // 110b
                        else
                            throw Exception::SemanticError("Invalid 16-bit base register", -1, -1);
                    }
                    else if (!mem.use_base_reg && mem.use_index_reg)
                    {
                        // [si], [di]
                        if (mem.index_reg == Registers::SI) addressing_mode = 4;  // 100b
                        else if (mem.index_reg == Registers::DI) addressing_mode = 5;  // 101b
                        else
                            throw Exception::SemanticError("Invalid 16-bit index register", -1, -1);
                    }
                    else
                    {
                        addressing_mode = 6;  // Direct address
                    }

                    mod_rm = addressing_mode;

                    if (mem.use_displacement)
                    {
                        use_displacement = true;
                        displacement_immediate = mem.displacement;

                        if (addressing_mode == 6)  // Direct address
                            mod_mod = Mod::INDIRECT;
                        else
                            mod_mod = Mod::INDIRECT_DISP32;
                    }
                    else
                    {
                        mod_mod = Mod::INDIRECT;
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

                    if (mem.use_base_reg)
                    {
                        auto [baseI, baseUseREX, baseSetREX] = getReg(mem.base_reg);

                        if (baseUseREX) useREX = true;
                        if (baseSetREX) rexB = true;

                        mod_rm = baseI;

                        sib_index = SIB_NoIndex;
                        if (baseI == modRMSIB) useSIB = true;

                        if (baseI == modRMDisp)
                        {
                            mod_mod = Mod::INDIRECT_DISP8;
                        }
                    }
                    else if (mem.use_index_reg)
                    {
                        mod_mod = Mod::INDIRECT;
                        mod_rm = modRMDisp;
                        
                        useSIB = true;
                        sib_index = SIB_NoIndex;
                    }

                    if (mem.use_index_reg)
                    {
                        auto [indexI, indexUseREX, indexSetREX] = getReg(mem.index_reg);

                        if (indexI == SIB_NoIndex && !indexSetREX)
                            throw Exception::SemanticError("Can't use ESP/RSP as index register", -1, -1);

                        if (indexUseREX) useREX = true;
                        if (indexSetREX) rexX = true;

                        useSIB = true;
                        sib_index = indexI;

                        scale_immediate = mem.scale;

                        use_sib_scale = true;
                    }

                    if (mem.use_displacement)
                    {
                        if (mem.use_base_reg) mod_mod = Mod::INDIRECT_DISP32;

                        use_displacement = true;
                        displacement_immediate = mem.displacement;

                        if (!mem.use_base_reg && !mem.use_index_reg)
                        {
                            if (bits == BitMode::Bits64)
                            {
                                mod_mod = Mod::INDIRECT;
                                mod_rm = modRMDisp;
                            
                                useSIB = true;
                                sib_scale = Scale::x1;
                                sib_index = SIB_NoIndex;
                            }
                            else
                            {
                                mod_mod = Mod::INDIRECT;
                                mod_rm = modRMDisp;
                            }
                        }
                    }
                }

                if (use_sib_scale)
                {
                    ::Encoder::Evaluation scale_evaluation = Evaluate(scale_immediate);

                    if (scale_evaluation.useOffset)
                        throw Exception::SemanticError("Scale must be a constant", -1, -1); // TODO

                    Int128 scale_result = scale_evaluation.result;

                    switch (scale_result)
                    {
                        case 1: sib_scale = Scale::x1; break;
                        case 2: sib_scale = Scale::x2; break;
                        case 4: sib_scale = Scale::x4; break;
                        case 8: sib_scale = Scale::x8; break;
                        default:
                            throw Exception::SemanticError("Scale must be 1, 2, 4, or 8", -1, -1); // TODO
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
                    case Instructions::NOT: mod_reg = 2; break;
                    case Instructions::NEG: mod_reg = 3; break;

                    case Instructions::INC: mod_reg = 0; break;
                    case Instructions::DEC: mod_reg = 1; break;
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
                        useREX = true;
                        rexW = true;
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
            // TODO
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (use16BitAddressPrefix) currentOffset++;
            if (useREX) currentOffset++;
            if (useModRM) currentOffset++;
            if (useSIB) currentOffset++;

            displacement_value = evaluation.offset; // TODO: Check for overflow

            ::Encoder::RelocationSize relocSize;
            if (mod_mod == Mod::INDIRECT_DISP8)
            {
                relocSize = ::Encoder::RelocationSize::Bit8;
            }
            else // DISP32 or mod_rm = modRMDisp
            {
                if (bitmode == BitMode::Bits16) relocSize = ::Encoder::RelocationSize::Bit16;
                else                            relocSize = ::Encoder::RelocationSize::Bit32;
            }

            AddRelocation(
                currentOffset,
                evaluation.offset,
                true,
                evaluation.usedSection,
                ::Encoder::RelocationType::Absolute,
                relocSize,
                is_displacement_signed,
                evaluation.isExtern
            );
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

    if (useREX) instr.push_back(::x86::getRex(rexW, rexR, rexX, rexB));

    instr.push_back(opcode);

    if (useModRM)
    {
        if (useSIB) instr.push_back(getModRM(mod_mod, mod_reg, modRMSIB));
        else        instr.push_back(getModRM(mod_mod, mod_reg, mod_rm));
    }

    if (useSIB) instr.push_back(getSIB(sib_scale, sib_index, mod_rm));

    bool directAddressing = false;
    if (use16BitAddressing) directAddressing = (mod_mod == Mod::INDIRECT && mod_rm == 6);
    else                    directAddressing = (mod_mod == Mod::INDIRECT && mod_rm == modRMDisp);

    // TODO: FIXME: EVERYTHING
    if (mod_mod == Mod::INDIRECT_DISP8)
    {
        instr.push_back(static_cast<uint8_t>(displacement_value));
    }
    else if (mod_mod == Mod::INDIRECT_DISP32 || directAddressing)
    {
        uint32_t sizeInBytes;
        if (use16BitAddressing) sizeInBytes = 2;
        else                    sizeInBytes = 4;

        uint64_t oldSize = instr.size();
        instr.resize(oldSize + sizeInBytes);

        std::memcpy(instr.data() + oldSize, &displacement_value, sizeInBytes);
    }

    return instr;
}

uint64_t x86::Argument_ALU_Instruction::size()
{
    uint64_t s = 1;

    if (use16BitPrefix) s++;
    if (use16BitAddressPrefix) s++;

    if (useREX) s++;

    if (useModRM) s++;

    if (useSIB) s++;

    bool directAddressing = false;
    if (use16BitAddressing) directAddressing = (mod_mod == Mod::INDIRECT && mod_rm == 6);
    else                    directAddressing = (mod_mod == Mod::INDIRECT && mod_rm == modRMDisp);

    // TODO: FIXME: EVERYTHING
    if      (mod_mod == Mod::INDIRECT_DISP8)  s++;
    else if (mod_mod == Mod::INDIRECT_DISP32 || directAddressing)
    {
        if (use16BitAddressing) s += 2;
        else                    s += 4;
    }

    return s;
}
