#include "stack.hpp"

#include <limits>
#include <cstring>

x86::Normal_Stack_Instruction::Normal_Stack_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands)
    : ::x86::Instruction(e, bits)
{
    switch (mnemonic)
    {
        case Instructions::PUSH: case Instructions::POP:
        {
            if (operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for argument ALU instruction", -1, -1);

            Parser::Instruction::Operand operand = operands[0];

            if (std::holds_alternative<Parser::Immediate>(operand))
            {
                if (mnemonic == POP)
                    throw Exception::SyntaxError("Can't use pop with an immediate operand", -1, -1);

                useImmediate = true;
                immediate = std::get<Parser::Immediate>(operand);

                canOptimize = true;

                opcode = 0x68;

                switch (bits)
                {
                    case BitMode::Bits16:
                        immediateSpecific.max = std::numeric_limits<uint16_t>::max();
                        immediateSpecific.sizeInBits = 16;
                        break;

                    case BitMode::Bits32: case BitMode::Bits64:
                        immediateSpecific.max = std::numeric_limits<uint32_t>::max();
                        immediateSpecific.sizeInBits = 32;
                        break;

                    default:
                        throw Exception::InternalError("Invalid bitmode", -1, -1);
                }
            }
            else
            {
                if (std::holds_alternative<Parser::Instruction::Register>(operand))
                {
                    Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(operand);

                    if (!isGPR(reg.reg))
                        throw Exception::SemanticError("Argument ALU instruction only accepts GPRs", -1, -1);

                    uint8_t size = getRegSize(reg.reg, bits);

                    switch (size)
                    {
                        case 16:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;
                            break;

                        case 32:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;
                            else if (bits == BitMode::Bits64)
                                throw Exception::SyntaxError("Can't push/pop 32-bit registers in 64-bit mode", -1, -1);
                            break;

                        case 64:
                            if (bits != BitMode::Bits64)
                                throw Exception::SyntaxError("Can't push/pop 64-bit registers in 16/32-bit mode", -1, -1);
                            break;
                        
                        case 8:
                            throw Exception::SyntaxError("Can't push/pop 8-bit registers", -1, -1);

                        default:
                            throw Exception::InternalError("Invalid bitmode", -1, -1);
                    }

                    auto [idx, setREX, useREX] = getReg(reg.reg);

                    if (useREX) rex.use = true;
                    if (setREX) rex.b = true;

                    if (mnemonic == PUSH) opcode = 0x50 + idx;
                    else                  opcode = 0x58 + idx;
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(operand))
                {
                    Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(operand);
                    uint64_t size = parseMemory(mem, bits, true);

                    switch (size)
                    {
                        case 16:
                            if (bits != BitMode::Bits16) use16BitPrefix = true;
                            break;

                        case 32:
                            if (bits == BitMode::Bits16) use16BitPrefix = true;
                            else if (bits == BitMode::Bits64)
                                throw Exception::SyntaxError("Can't push/pop 32-bit registers in 64-bit mode", -1, -1);
                            break;

                        case 64:
                            if (bits != BitMode::Bits64)
                                throw Exception::SyntaxError("Can't push/pop 64-bit registers in 16/32-bit mode", -1, -1);
                            break;
                        
                        case 8:
                            throw Exception::SyntaxError("Can't push/pop 8-bit registers", -1, -1);

                        default:
                            throw Exception::InternalError("Invalid bitmode", -1, -1);
                    }

                    if (mnemonic == PUSH)
                    {
                        opcode = 0xFF;

                        modrm.use = true;
                        modrm.reg = 6;
                    }
                    else if (mnemonic == POP)
                    {
                        opcode = 0x8F;
                    }
                }
            }
        }
    }
}

void x86::Normal_Stack_Instruction::evaluateS()
{
    if (useImmediate)
    {
        ::Encoder::Evaluation evaluation = Evaluate(immediate, false, 0);

        if (evaluation.useOffset)
        {
            usedReloc = true;
            relocUsedSection = evaluation.usedSection;
            relocIsExtern = evaluation.isExtern;
            immediateSpecific.value = evaluation.offset;
        }
        else
        {
            Int128 result = evaluation.result;

            // FIXME
            //if (result > specific.alu_reg_imm.max) throw Exception::SemanticError("Operand too large for instruction", -1, -1);

            immediateSpecific.value = static_cast<uint64_t>(result);
        }
    }
}

bool x86::Normal_Stack_Instruction::optimizeS()
{
    if (canOptimize)
    {
        int32_t value;

        if (bitmode == BitMode::Bits16)
        {
            const int16_t val16 = static_cast<int16_t>(static_cast<uint16_t>(immediateSpecific.value));
            value = static_cast<int32_t>(val16);
        }
        else
            value = static_cast<int32_t>(static_cast<uint32_t>(immediateSpecific.value));

        if (
            value <= static_cast<int64_t>(std::numeric_limits<int8_t>::max()) &&
            value >= static_cast<int64_t>(std::numeric_limits<int8_t>::min())
        )
        {
            canOptimize = false;

            opcode = 0x6A;

            immediateSpecific.max = std::numeric_limits<uint8_t>::max();
            immediateSpecific.sizeInBits = 8;

            return true;
        }
    }

    return false;
}

void x86::Normal_Stack_Instruction::encodeS(std::vector<uint8_t>& buffer)
{
    if (useImmediate)
    {
        uint32_t sizeInBytes = immediateSpecific.sizeInBits / 8;

        uint64_t oldSize = buffer.size();
        buffer.resize(oldSize + sizeInBytes);

        std::memcpy(buffer.data() + oldSize, &immediateSpecific.value, sizeInBytes);

        if (usedReloc)
        {
            uint64_t currentOffset = 1; // opcode
            if (use16BitPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (modrm.use) currentOffset++;

            ::Encoder::RelocationSize relocSize;
            switch (immediateSpecific.sizeInBits)
            {
                case 8: relocSize = ::Encoder::RelocationSize::Bit8; break;
                case 16: relocSize = ::Encoder::RelocationSize::Bit16; break;
                case 32: relocSize = ::Encoder::RelocationSize::Bit32; break;
                case 64: relocSize = ::Encoder::RelocationSize::Bit64; break;
                default: throw Exception::InternalError("Unknown size in bits " + std::to_string(immediateSpecific.sizeInBits), -1, -1);
            }

            AddRelocation(
                currentOffset,
                immediateSpecific.value,
                true,
                relocUsedSection,
                ::Encoder::RelocationType::Absolute,
                relocSize,
                false, // TODO: Check if signed
                relocIsExtern
            );
        }
    }
}

uint64_t x86::Normal_Stack_Instruction::sizeS()
{
    if (useImmediate)
        return immediateSpecific.sizeInBits / 8;
    else
        return 0;
}
