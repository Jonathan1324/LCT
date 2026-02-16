#include "interrupt.hpp"

x86::Argument_Interrupt_Instruction::Argument_Interrupt_Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr)
    : ::x86::Instruction(e, instr)
{
    switch (instr.mnemonic)
    {
        case Instructions::INT:
        {
            opcode = 0xCD;

            if (instr.operands.empty())
                throw Exception::InternalError("'int' requires one argument", -1, -1, nullptr);
            if (!std::holds_alternative<Parser::Immediate>(instr.operands[0]))
                throw Exception::InternalError("'int': wrong operand type", -1, -1, nullptr);

            immediate.use = true;
            immediate.immediate = std::get<Parser::Immediate>(instr.operands[0]);

            immediate.sizeInBits = 8;

            break;
        }

        default:
            throw Exception::InternalError("Unknown argument control instruction", -1, -1, nullptr);
    }
}
