#include "Encoder.hpp"

uint64_t x86::Instruction::parseMemory(
    const Parser::Instruction::Memory& mem,
    BitMode bitmode,
    bool expectSize
) {
    // TODO: RIP-Relative

    modrm.use = true;
    modrm.mod = Mod::INDIRECT;

    uint64_t memSize = mem.pointer_size;

    if (expectSize && memSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
        throw Exception::SyntaxError("Pointer size not specified for memory operand", -1, -1);

    uint8_t mem_reg_size;

    if (mem.use_reg1 && mem.use_reg2)
    {
        uint8_t base_reg_size = getRegSize(mem.reg1, bitmode);
        uint8_t index_reg_size = getRegSize(mem.reg2, bitmode);
        if (base_reg_size != index_reg_size)
            throw Exception::SyntaxError("Base register and index register have different sizes", -1, -1);
        mem_reg_size = base_reg_size;
    }
    else if (mem.use_reg1 && !mem.use_reg2)
    {
        mem_reg_size = getRegSize(mem.reg1, bitmode);
    }
    else if (mem.use_reg2 && !mem.use_reg1)
    {
        mem_reg_size = getRegSize(mem.reg2, bitmode);
    }
    else
    {
        if (bitmode == BitMode::Bits16)      mem_reg_size = 16;
        else if (bitmode == BitMode::Bits32) mem_reg_size = 32;
        else                              mem_reg_size = 64;
    }

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

    bool hasBase = false;
    uint64_t baseReg = 0;

    bool hasIndex = false;
    uint64_t indexReg = 0;

    uint64_t scale = 0;

    // FIXME: Fix
    if (mem.use_reg1 && mem.use_reg2)
    {
        ::Encoder::Evaluation reg1_evaluation = Evaluate(mem.scale1, false, 0);
        ::Encoder::Evaluation reg2_evaluation = Evaluate(mem.scale2, false, 0);

        if (reg1_evaluation.useOffset || reg2_evaluation.useOffset)
            throw Exception::SemanticError("Scale must be a constant", -1, -1); // TODO

        Int128& reg1_result = reg1_evaluation.result;
        Int128& reg2_result = reg2_evaluation.result;

        hasBase = true;
        hasIndex = true;

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
        else if (reg1_result != 1 && reg2_result == 1)
        {
            baseReg = mem.reg2;

            indexReg = mem.reg1;
            scale = static_cast<uint64_t>(reg1_result);
        }
        else
            throw Exception::SemanticError("Can't have 2 index registers", -1, -1);
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

        ::Encoder::Evaluation evaluation = Evaluate(*usedScale, false, 0);

        if (evaluation.useOffset)
            throw Exception::SemanticError("Scale must be a constant", -1, -1); // TODO

        Int128& result = evaluation.result;

        if (addressMode == AddressMode::Bits16)
        {
            scale = static_cast<uint64_t>(result); // TODO
            
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
        else // 32,64
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

    if (hasBase)
    {
        if (!isGPR(baseReg))
            throw Exception::SyntaxError("Only GPRs are allowed as memory operands", -1, -1);
    }
    if (hasIndex)
    {
        if (!isGPR(indexReg))
            throw Exception::SyntaxError("Only GPRs are allowed as memory operands", -1, -1);

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

    if (addressMode == AddressMode::Bits16)
    {
        if (bitmode == BitMode::Bits64)
            throw Exception::SemanticError("Can't use 16 bit registers in 64 bit mode", -1, -1);
        else if (bitmode == BitMode::Bits32)
            use16BitAddressPrefix = true;

        if (hasBase && (baseReg != Registers::BX && baseReg != Registers::BP))
            throw Exception::SemanticError("Only bx/bp allowed as 16-bit base", -1, -1);
        
        if (hasIndex && (indexReg != Registers::SI && indexReg != Registers::DI))
            throw Exception::SemanticError("Only si/di allowed as 16-bit index", -1, -1);

        bool noBase = false;
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
            else if (baseReg == Registers::BP) addressing_mode = 6; // 110b
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
            noBase = true;
            addressing_mode = 6;  // Direct address
        }

        modrm.rm = addressing_mode;

        if (mem.use_displacement)
        {
            displacement.use = true;
            displacement.immediate = mem.displacement;

            if (noBase)  // Direct address
            {
                modrm.mod = Mod::INDIRECT;
                displacement.can_optimize = false;
                displacement.direct_addressing = true;
            }
            else if (!hasBase && hasIndex)
            {
                modrm.mod = Mod::INDIRECT_DISP8;
            }
            else
                modrm.mod = Mod::INDIRECT_DISP32;
        }
        else
        {
            if (addressing_mode == 6)
            {
                modrm.mod = Mod::INDIRECT_DISP8;
            }
            else
            {
                modrm.mod = Mod::INDIRECT;
            }
        }
    }
    else // 32,64
    {
        if (addressMode == AddressMode::Bits32 && bitmode != BitMode::Bits32)
            use16BitAddressPrefix = true;

        if (addressMode == AddressMode::Bits64)
        {
            if (bitmode != BitMode::Bits64)
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

                if (addressMode == AddressMode::Bits64)
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

    checkSize(memSize, bitmode);

    return memSize;
}

uint64_t x86::Instruction::parseRegister(
    const Parser::Instruction::Register& reg,
    BitMode bitmode,
    bool isReg
) {
    checkReg(reg, bitmode);

    modrm.use = true;

    auto [regIndex, regUseREX, regSetREX] = getReg(reg.reg);
    uint8_t regSize = getRegSize(reg.reg, bitmode);

    if (regUseREX) rex.use = true;
    if (regSetREX)
    {
        if (isReg) rex.r = true;
        else       rex.b = true;
    }

    if (isReg) modrm.reg = regIndex;
    else       modrm.rm = regIndex;

    checkSize(regSize, bitmode);

    return regSize;
}
