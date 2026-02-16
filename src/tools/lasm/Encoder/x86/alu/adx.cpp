#include "alu.hpp"

#include <limits>
#include <cstring>

x86::ADX_ALU_Instruction::ADX_ALU_Instruction(::Encoder::Encoder& e, const Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::ADCX: case Instructions::ADOX:
        {
            if (instr.operands.size() != 2)
                throw Exception::InternalError("Wrong operand count for ADX ALU instruction", -1, -1);

            if (!std::holds_alternative<Parser::Instruction::Register>(instr.operands[0]))
                throw Exception::InternalError("Wrong operand type for ADX ALU instruction", -1, -1);

            const Parser::Instruction::Register& regOperand = std::get<Parser::Instruction::Register>(instr.operands[0]);

            if (instr.mnemonic == ADCX)
                use66OpcodeOverride = true;
            else // ADOX
                useF3OpcodeOverride = true;
            
            opcodeEscape = OpcodeEscape::THREE_BYTE_38;

            opcode = 0xF6;

            uint64_t mainSize = parseRegister(regOperand, bits, true);
            if (!isGPR(regOperand.reg))
                throw Exception::SemanticError("ADX ALU instruction only accepts GPRs", -1, -1);

            uint64_t otherSize;
            if (std::holds_alternative<Parser::Instruction::Register>(instr.operands[1]))
            {
                Parser::Instruction::Register reg = std::get<Parser::Instruction::Register>(instr.operands[1]);
                otherSize = parseRegister(reg, bits, false);

                if (!isGPR(reg.reg))
                    throw Exception::SemanticError("ADX ALU instruction only accepts GPRs", -1, -1);
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(instr.operands[1]))
            {
                Parser::Instruction::Memory mem = std::get<Parser::Instruction::Memory>(instr.operands[1]);
                otherSize = parseMemory(mem, bits, false);

                if (otherSize == Parser::Instruction::Memory::NO_POINTER_SIZE)
                    otherSize = mainSize;
            }
            else
                throw Exception::InternalError("Wrong operand type for ADX ALU instruction", -1, -1);

            if (mainSize != otherSize)
                throw Exception::SemanticError("Can't use instruction with operands of different size", -1, -1);

            switch (mainSize)
            {
                case 32:
                    break;

                case 64:
                    rex.use = true;
                    rex.w = true;
                    break;

                default:
                    throw Exception::SemanticError("ADX ALU instruction only accepts 32 and 64 operands", -1, -1);
            }

            break;
        }

        default:
            throw Exception::InternalError("Unknown ADX ALU instruction", -1, -1, nullptr);
    }
}
