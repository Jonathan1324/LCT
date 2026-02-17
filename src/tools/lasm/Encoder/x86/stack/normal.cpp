#include "stack.hpp"

#include <limits>
#include <cstring>

x86::Normal_Stack_Instruction::Normal_Stack_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::PUSH: case Instructions::POP:
        {
            if (instr.operands.size() != 1)
                throw Exception::InternalError("Wrong argument count for argument ALU instruction", -1, -1);

            Parser::Instruction::Operand operand = instr.operands[0];

            if (std::holds_alternative<Parser::Immediate>(operand))
            {
                if (instr.mnemonic == POP)
                    throw Exception::SyntaxError("Can't use pop with an immediate operand", -1, -1);

                immediate.use = true;
                immediate.immediate = std::get<Parser::Immediate>(operand);
                
                if (bits == BitMode::Bits64) immediate.is_signed = true;

                canOptimize = true;

                opcode = 0x68;

                switch (bits)
                {
                    case BitMode::Bits16:
                        immediate.sizeInBits = 16;
                        break;

                    case BitMode::Bits32: case BitMode::Bits64:
                        immediate.sizeInBits = 32;
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

                    if (instr.mnemonic == PUSH) opcode = 0x50 + idx;
                    else                        opcode = 0x58 + idx;
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

                    if (instr.mnemonic == PUSH)
                    {
                        opcode = 0xFF;

                        modrm.use = true;
                        modrm.reg = 6;
                    }
                    else // if (instr.mnemonic == POP)
                    {
                        opcode = 0x8F;
                    }
                }
            }
        }
    }
}

bool x86::Normal_Stack_Instruction::optimizeS()
{
    if (canOptimize && !immediate.needsRelocation)
    {
        int32_t value;

        if (bits == BitMode::Bits16)
        {
            const int16_t val16 = static_cast<int16_t>(static_cast<uint16_t>(immediate.value));
            value = static_cast<int32_t>(val16);
        }
        else
            value = static_cast<int32_t>(static_cast<uint32_t>(immediate.value));

        if (
            value <= static_cast<int64_t>(std::numeric_limits<int8_t>::max()) &&
            value >= static_cast<int64_t>(std::numeric_limits<int8_t>::min())
        )
        {
            canOptimize = false;

            opcode = 0x6A;

            immediate.sizeInBits = 8;

            return true;
        }
    }

    return false;
}
