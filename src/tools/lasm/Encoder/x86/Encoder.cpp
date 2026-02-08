#include "Encoder.hpp"

#include "control.hpp"
#include "interrupt.hpp"
#include "flag.hpp"
#include "stack.hpp"
#include "data.hpp"
#include "alu.hpp"

x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> x86::Encoder::EncodePadding(size_t length)
{
    std::vector<uint8_t> buffer(length, 0x90);  // TODO: not cool
    return buffer;
}

::Encoder::Encoder::Instruction* x86::Encoder::GetInstruction(const Parser::Instruction::Instruction& instruction)
{
    switch (instruction.mnemonic)
    {
        // CONTROL
        case Instructions::NOP:
        case Instructions::HLT:
            return new x86::Simple_Control_Instruction(*this, instruction.bits, instruction.mnemonic);

        // INTERRUPT
        case Instructions::INT:
            return new x86::Argument_Interrupt_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::IRET: case Instructions::IRETQ:
        case Instructions::IRETD: case Instructions::SYSCALL:
        case Instructions::SYSRET: case Instructions::SYSENTER:
        case Instructions::SYSEXIT:
            return new x86::Simple_Interrupt_Instruction(*this, instruction.bits, instruction.mnemonic);

        // FLAGS
        case Instructions::CLC: case Instructions::STC: case Instructions::CMC:
        case Instructions::CLD: case Instructions::STD:
        case Instructions::CLI: case Instructions::STI:
        case Instructions::LAHF: case Instructions::SAHF:
            return new x86::Simple_Flag_Instruction(*this, instruction.bits, instruction.mnemonic);
    
        // STACK
        case Instructions::PUSHA: case Instructions::POPA:
        case Instructions::PUSHAD: case Instructions::POPAD:
        case Instructions::PUSHF: case Instructions::POPF:
        case Instructions::PUSHFD: case Instructions::POPFD:
        case Instructions::PUSHFQ: case Instructions::POPFQ:
            return new x86::Simple_Stack_Instruction(*this, instruction.bits, instruction.mnemonic);

        // DATA
        case Instructions::MOV:
            return new x86::Mov_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        // ALU
        case Instructions::ADD: case Instructions::ADC: case Instructions::SUB:
        case Instructions::SBB: case Instructions::CMP: case Instructions::TEST:
        case Instructions::AND: case Instructions::OR: case Instructions::XOR:
            return new x86::Two_Argument_ALU_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::MUL: case Instructions::IMUL:
        case Instructions::DIV: case Instructions::IDIV:
            return new x86::Mul_Div_ALU_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::SHL: case Instructions::SHR:
        case Instructions::SAL: case Instructions::SAR:
        case Instructions::ROL: case Instructions::ROR:
        case Instructions::RCL: case Instructions::RCR:
            return new x86::Shift_Rotate_ALU_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::NOT: case Instructions::NEG:
        case Instructions::INC: case Instructions::DEC:
            return new x86::Argument_ALU_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);
    }

    return nullptr;
}
