#include "control.hpp"

#include <limits>
#include <cstring>

x86::CALL_Instruction::CALL_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    if (instr.mnemonic != CALL)
        throw Exception::InternalError("Unknown call instruction", -1, -1);

    if (instr.operands.size() != 1)
        throw Exception::InternalError("Wrong argument count for call instruction", -1, -1);

    const Parser::Instruction::Operand& operand = instr.operands[0];

    if (std::holds_alternative<Parser::Immediate>(operand))
    {
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

        opcode = 0xE8;
    }
    else
    {
        opcode = 0xFF;

        uint64_t size;
        if (std::holds_alternative<Parser::Instruction::Register>(operand))
        {
            Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);
            size = parseRegister(reg, bits, false);

            if (!isGPR(reg.reg))
                throw Exception::SyntaxError("Can only jmp to GPRs", -1, -1);
        }
        else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
        {
            Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
            size = parseMemory(mem, bits, false);
        }

        switch (size)
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

        modrm.use = true;
        modrm.reg = 2;
    }
}

void x86::CALL_Instruction::evaluateS()
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

void x86::CALL_Instruction::encodeS(std::vector<uint8_t>& buffer)
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

uint64_t x86::CALL_Instruction::sizeS()
{
    if (useImmediate)
        return sizeInBits / 8;
    else
        return 0;
}
