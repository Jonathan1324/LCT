#include "alu.hpp"

#include <limits>
#include <cstring>

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

                switch (mem_reg_size)
                {
                    case 16:
                        addressMode = AddressMode::Bits16;
                        break;

                    case 32:
                        addressMode = AddressMode::Bits32;
                        break;

                    case 64:
                        addressMode = AddressMode::Bits64;
                        break;

                    default:
                        throw Exception::SemanticError("Memory operand registers need to be 16, 32 or 64 bits in size", -1, -1);
                }

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

                    if (addressMode == AddressMode::Bits16)
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
                    if (addressMode == AddressMode::Bits16)
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
                        displacement.use = true;
                        displacement.immediate = mem.displacement;

                        if (addressing_mode == 6)  // Direct address
                        {
                            modrm.mod = Mod::INDIRECT;
                            displacement.can_optimize = false;
                            displacement.direct_addressing = true;
                        }
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
                        displacement.is_signed = true;
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

                        displacement.use = true;
                        displacement.immediate = mem.displacement;

                        if (!hasBase && !hasIndex)
                        {
                            displacement.direct_addressing = true;

                            if (bits == BitMode::Bits64)
                            {
                                modrm.mod = Mod::INDIRECT;
                                modrm.rm = modRMDisp;
                            
                                sib.use = true;
                                sib.scale = Scale::x1;
                                sib.index = SIB_NoIndex;

                                displacement.can_optimize = false;
                            }
                            else
                            {
                                modrm.mod = Mod::INDIRECT;
                                modrm.rm = modRMDisp;

                                displacement.can_optimize = false;
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
