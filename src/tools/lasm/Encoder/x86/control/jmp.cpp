#include "control.hpp"

#include <limits>
#include <cstring>

x86::JMP_Instruction::JMP_Instruction(::Encoder::Encoder &e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    mnemonicI = mnemonic;
    switch (mnemonic)
    {
        case JMP:
        {
            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for jmp instruction", -1, -1);

            Parser::Instruction::Operand& operand = operands[0];

            if (std::holds_alternative<Parser::Instruction::Register>(operand))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);

                if (!isGPR(reg.reg))
                    throw Exception::SyntaxError("Can only jmp to GPRs", -1, -1);

                uint8_t regSize = getRegSize(reg.reg, bits);

                switch (regSize)
                {
                    case 8:
                        throw Exception::SemanticError("Can't jump to 8-bit registers", -1, -1);

                    case 16:
                        if (bits == BitMode::Bits32) use16BitPrefix = true;
                        if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 16-bit registers in 32/64 bit mode", -1, -1);
                        break;

                    case 32:
                        if (bits == BitMode::Bits16) use16BitPrefix = true;
                        if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 32-bit registers in 16/64 bit mode", -1, -1);
                        break;

                    case 64:
                        if (bits == BitMode::Bits64) break;
                        throw Exception::SemanticError("Can't jump to 32-bit registers in 16/32 bit mode", -1, -1);
                }

                auto [regIndex, useREX, setREX] = getReg(reg.reg);

                if (useREX) rex.use = true;
                if (setREX) rex.b = true;

                opcode = 0xFF;

                modrm.use = true;
                modrm.reg = 4;
                modrm.rm = regIndex;

                break;
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                uint64_t memSize = parseMemory(mem, bits, false);

                switch (memSize)
                {
                    case Parser::Instruction::Memory::NO_POINTER_SIZE:
                        break;

                    case 8:
                        throw Exception::SemanticError("Can't jump to 8-bit registers", -1, -1);

                    case 16:
                        if (bits == BitMode::Bits32) use16BitPrefix = true;
                        if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 16-bit registers in 32/64 bit mode", -1, -1);
                        break;

                    case 32:
                        if (bits == BitMode::Bits16) use16BitPrefix = true;
                        if (bits == BitMode::Bits64) throw Exception::SemanticError("Can't jump to 32-bit registers in 16/64 bit mode", -1, -1);
                        break;

                    case 64:
                        if (bits == BitMode::Bits64) break;
                        throw Exception::SemanticError("Can't jump to 32-bit registers in 16/32 bit mode", -1, -1);
                }

                opcode = 0xFF;
                modrm.use = true;
                modrm.reg = 4;

                break;
            }

            // Same logic when immediate
        }

        case JE: case JNE:
        case JG: case JGE: case JL: case JLE:
        case JA: case JAE: case JB: case JBE:
        case JO: case JNO: case JS: case JNS:
        case JP: case JNP: case JC: case JNC:
        {
            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for jmp instruction", -1, -1);

            Parser::Instruction::Operand& operand = operands[0];

            if (!std::holds_alternative<Parser::Immediate>(operand))
                throw Exception::SyntaxError("Only immediates are allowed for conditional jumps", -1, -1);

            useImmediate = true;
            canOptimize = true;

            switch (bits)
            {
                case BitMode::Bits16:
                    max = std::numeric_limits<int16_t>::max();
                    sizeInBits = 16;

                    instrSize = 3; // opcode + off16
                    break;

                case BitMode::Bits32: case BitMode::Bits64:
                    max = std::numeric_limits<int32_t>::max();
                    sizeInBits = 32;

                    instrSize = 5; // opcode + off32
                    break;
            }

            immediate = std::get<Parser::Immediate>(operand);

            switch (mnemonic)
            {
                case JMP: opcode = 0xE9; break;

                case JE: useOpcodeEscape = true; opcode = 0x84; break;
                case JNE: useOpcodeEscape = true; opcode = 0x85; break;

                case JG: useOpcodeEscape = true; opcode = 0x8F; break;
                case JGE: useOpcodeEscape = true; opcode = 0x8D; break;
                case JL: useOpcodeEscape = true; opcode = 0x8C; break;
                case JLE: useOpcodeEscape = true; opcode = 0x8E; break;

                case JA: useOpcodeEscape = true; opcode = 0x87; break;
                case JAE: useOpcodeEscape = true; opcode = 0x83; break;
                case JB: useOpcodeEscape = true; opcode = 0x82; break;
                case JBE: useOpcodeEscape = true; opcode = 0x86; break;

                case JO: useOpcodeEscape = true; opcode = 0x80; break;
                case JNO: useOpcodeEscape = true; opcode = 0x81; break;
                case JS: useOpcodeEscape = true; opcode = 0x88; break;
                case JNS: useOpcodeEscape = true; opcode = 0x89; break;

                case JP: useOpcodeEscape = true; opcode = 0x8A; break;
                case JNP: useOpcodeEscape = true; opcode = 0x8B; break;
                case JC: useOpcodeEscape = true; opcode = 0x82; break;
                case JNC: useOpcodeEscape = true; opcode = 0x83; break;
            }

            if (useOpcodeEscape) instrSize++;
        }
    }
}

void x86::JMP_Instruction::evaluateS()
{
    if (useImmediate)
    {
        ::Encoder::Evaluation evaluation = Evaluate(immediate, true, instrSize);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            relocUsedSection = evaluation.usedSection;
            relocIsExtern = evaluation.isExtern;
            value = evaluation.offset;
        }
        else
        {
            Int128 result = evaluation.result;

            // FIXME
            //if (result > max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

            value = static_cast<uint64_t>(result);
        }
    }
}

// TODO: Labels and Sections

bool x86::JMP_Instruction::optimizeS()
{
    if (canOptimize && !usedReloc)
    {
        int32_t offset;
        if (sizeInBits == 16)
            offset = static_cast<int32_t>(static_cast<int16_t>(static_cast<uint16_t>(value)));
        else
            offset = static_cast<int32_t>(static_cast<uint32_t>(value));

        if (
            offset <= static_cast<int64_t>(std::numeric_limits<int8_t>::max()) &&
            offset >= static_cast<int64_t>(std::numeric_limits<int8_t>::min())
        )
        {
            canOptimize = false;

            useOpcodeEscape = false;

            switch (mnemonicI)
            {
                case JMP: opcode = 0xEB; break;

                case JE: opcode = 0x74; break;
                case JNE: opcode = 0x75; break;

                case JG: opcode = 0x7F; break;
                case JGE: opcode = 0x7D; break;
                case JL: opcode = 0x7C; break;
                case JLE: opcode = 0x7E; break;

                case JA: opcode = 0x77; break;
                case JAE: opcode = 0x73; break;
                case JB: opcode = 0x72; break;
                case JBE: opcode = 0x76; break;

                case JO: opcode = 0x70; break;
                case JNO: opcode = 0x71; break;
                case JS: opcode = 0x78; break;
                case JNS: opcode = 0x79; break;

                case JP: opcode = 0x7A; break;
                case JNP: opcode = 0x7B; break;
                case JC: opcode = 0x72; break;
                case JNC: opcode = 0x73; break;
            }

            max = std::numeric_limits<int8_t>::max();
            sizeInBits = 8;

            instrSize = 2; // opcode + off8

            return true;
        }
    }

    return false;
}

void x86::JMP_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (useImmediate)
    {
        uint32_t sizeInBytes = sizeInBits / 8;

        uint64_t oldSize = buffer.size();
        buffer.resize(oldSize + sizeInBytes);

        std::memcpy(buffer.data() + oldSize, &value, sizeInBytes);

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (useOpcodeEscape) currentOffset++;
            if (modrm.use) currentOffset++;

            ::Encoder::RelocationSize relocSize;
            switch (sizeInBits)
            {
                case 8: relocSize = ::Encoder::RelocationSize::Bit8; break;
                case 16: relocSize = ::Encoder::RelocationSize::Bit16; break;
                case 32: relocSize = ::Encoder::RelocationSize::Bit32; break;
                case 64: relocSize = ::Encoder::RelocationSize::Bit64; break;
                default: throw Exception::InternalError("Unknown size in bits " + std::to_string(sizeInBits), -1, -1);
            }

            AddRelocation(
                currentOffset,
                value - (instrSize - currentOffset),
                true,
                relocUsedSection,
                ::Encoder::RelocationType::PC_Relative,
                relocSize,
                false, // TODO: Check if signed
                relocIsExtern
            );
        }
    }
}

uint64_t x86::JMP_Instruction::sizeS()
{
    if (useImmediate)
        return sizeInBits / 8;
    else
        return 0;
}
