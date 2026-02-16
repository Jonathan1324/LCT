#include "alu.hpp"

#include <limits>
#include <cstring>

x86::Two_Argument_ALU_Instruction::Two_Argument_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::ADD: case Instructions::ADC:
        case Instructions::SUB: case Instructions::SBB:
        case Instructions::CMP: case Instructions::TEST:
        case Instructions::AND: case Instructions::OR:
        case Instructions::XOR:
        {
            if (instr.operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for two argument alu instruction", -1, -1);

            mainOperand = instr.operands[0];
            const Parser::Instruction::Operand& otherOperand = instr.operands[1];

            bool useRMFirst = false;
            bool otherIsImmediate = false;

            if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
            {
                Parser::Instruction::Register mainReg = std::get<Parser::Instruction::Register>(mainOperand);

                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    useRMFirst = true;
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(otherOperand))
                {
                    useRMFirst = false;
                }
                else if (std::holds_alternative<Parser::Immediate>(otherOperand))
                {
                    otherIsImmediate = true;
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
            {
                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    useRMFirst = true;
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(otherOperand))
                {
                    throw Exception::SemanticError("Instruction can't have two memory operands", -1, -1);
                }
                else if (std::holds_alternative<Parser::Immediate>(otherOperand))
                {
                    otherIsImmediate = true;
                }
            }

            if (otherIsImmediate)
            {
                if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);

                    switch (reg.reg)
                    {
                        case AL:
                            immediate.sizeInBits = 8;

                            accumulatorReg = true;
                            break;
                        
                        case AX:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;

                            immediate.sizeInBits = 16;
                            
                            accumulatorReg = true;
                            break;
                        
                        case EAX:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;

                            immediate.sizeInBits = 32;

                            accumulatorReg = true;
                            break;

                        case RAX:
                            immediate.sizeInBits = 32;

                            rex.use = true;
                            rex.w = true;

                            accumulatorReg = true;
                            break;
                    }
                }

                immediate.use = true;
                immediate.immediate = std::get<Parser::Immediate>(otherOperand);

                if (accumulatorReg)
                {
                    if (immediate.sizeInBits == 8)
                    {
                        switch (instr.mnemonic)
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
                        if (instr.mnemonic != TEST) canOptimize = true;
                        switch (instr.mnemonic)
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
                    uint64_t mainSize;
                    if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                    {
                        Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                        mainSize = parseRegister(reg, bits, false);

                        if (!isGPR(reg.reg))
                            throw Exception::SemanticError("Two argument ALU instruction only accepts GPRs", -1, -1);
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
                    {
                        Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                        mainSize = parseMemory(mem, bits, true);
                    }

                    switch (mainSize)
                    {
                        case 8:
                            immediate.sizeInBits = 8;
                            break;

                        case 16:
                            immediate.sizeInBits = 16;

                            if (bits != BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

                        case 32:
                            immediate.sizeInBits = 32;

                            if (bits == BitMode::Bits16)
                                use16BitPrefix = true;
                            break;

                        case 64:
                            immediate.sizeInBits = 32;

                            rex.use = true;
                            rex.w = true;

                            break;

                        default:
                            throw Exception::InternalError("Invalid bits", -1, -1);
                    }

                    if (instr.mnemonic == TEST)
                    {
                        if (immediate.sizeInBits == 8)
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
                        if (immediate.sizeInBits == 8)
                        {
                            opcode = 0x80;
                        }
                        else
                        {
                            canOptimize = true;
                            opcode = 0x81;
                        }

                        switch (instr.mnemonic)
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
                    }
                }
            }
            else
            {
                uint64_t mainSize;
                uint64_t otherSize;

                if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                    mainSize = parseRegister(reg, bits, !useRMFirst);

                    if (!isGPR(reg.reg))
                        throw Exception::SemanticError("Two argument ALU instruction only accepts GPRs", -1, -1);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                    mainSize = parseMemory(mem, bits, false);
                }

                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(otherOperand);
                    otherSize = parseRegister(reg, bits, useRMFirst);

                    if (!isGPR(reg.reg))
                        throw Exception::SemanticError("Two argument ALU instruction only accepts GPRs", -1, -1);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(otherOperand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(otherOperand);
                    otherSize = parseMemory(mem, bits, false);

                    if (otherSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                        otherSize = mainSize;
                }

                if (mainSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                    mainSize = otherSize;

                if (mainSize != otherSize)
                    throw Exception::SemanticError("Can't use instruction with operands of different size", -1, -1);

                if (mainSize == 8)
                {
                    switch (instr.mnemonic)
                    {
                        case ADD:  opcode = (useRMFirst ? 0x00 : 0x02); break;
                        case OR:   opcode = (useRMFirst ? 0x08 : 0x0A); break;
                        case ADC:  opcode = (useRMFirst ? 0x10 : 0x12); break;
                        case SBB:  opcode = (useRMFirst ? 0x18 : 0x1A); break;
                        case AND:  opcode = (useRMFirst ? 0x20 : 0x22); break;
                        case SUB:  opcode = (useRMFirst ? 0x28 : 0x2A); break;
                        case XOR:  opcode = (useRMFirst ? 0x30 : 0x32); break;
                        case CMP:  opcode = (useRMFirst ? 0x38 : 0x3A); break;
                        case TEST: opcode = 0x84; break; // commutative
                    }
                }
                else
                {
                    switch (instr.mnemonic)
                    {
                        case ADD:  opcode = (useRMFirst ? 0x01 : 0x03); break;
                        case OR:   opcode = (useRMFirst ? 0x09 : 0x0B); break;
                        case ADC:  opcode = (useRMFirst ? 0x11 : 0x13); break;
                        case SBB:  opcode = (useRMFirst ? 0x19 : 0x1B); break;
                        case AND:  opcode = (useRMFirst ? 0x21 : 0x23); break;
                        case SUB:  opcode = (useRMFirst ? 0x29 : 0x2B); break;
                        case XOR:  opcode = (useRMFirst ? 0x31 : 0x33); break;
                        case CMP:  opcode = (useRMFirst ? 0x39 : 0x3B); break;
                        case TEST: opcode = 0x85; break; // commutative
                    }
                }

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
                        break;

                    default:
                        throw Exception::InternalError("Invalid operand size", -1, -1);
                }

                if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                    if (rex.use && (reg.reg == AH || reg.reg == CH ||
                                    reg.reg == DH || reg.reg == BH))
                        throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                }
                if (std::holds_alternative<Parser::Instruction::Register>(otherOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(otherOperand);
                    if (rex.use && (reg.reg == AH || reg.reg == CH ||
                                    reg.reg == DH || reg.reg == BH))
                        throw Exception::SemanticError("Can't use high 8-bit regs using new registers", -1, -1);
                }
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown two argument alu instruction", -1, -1, nullptr);
    }
}

bool x86::Two_Argument_ALU_Instruction::optimizeS()
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

            opcode = 0x83;

            immediate.sizeInBits = 8;

            if (accumulatorReg)
            {
                uint64_t mainSize;
                if (std::holds_alternative<Parser::Instruction::Register>(mainOperand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(mainOperand);
                    mainSize = parseRegister(reg, bits, false);

                    if (!isGPR(reg.reg))
                        throw Exception::SemanticError("Two argument ALU instruction only accepts GPRs", -1, -1);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(mainOperand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(mainOperand);
                    mainSize = parseMemory(mem, bits, true);
                }
            }

            return true;
        }
    }

    return false;
}
