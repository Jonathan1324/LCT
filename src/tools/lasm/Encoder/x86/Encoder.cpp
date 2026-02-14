#include "Encoder.hpp"

#include "ascii/ascii.hpp"
#include "alu/alu.hpp"
#include "control/control.hpp"
#include "data/data.hpp"
#include "flag/flag.hpp"
#include "interrupt/interrupt.hpp"
#include "stack/stack.hpp"

#include <cstring>

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
        // ASCII
        case Instructions::AAA: case Instructions::AAD:
        case Instructions::AAM: case Instructions::AAS:
            return new x86::ASCII_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        // CONTROL
        case Instructions::NOP:
        case Instructions::HLT:
            return new x86::Simple_Control_Instruction(*this, instruction.bits, instruction.mnemonic);

        case Instructions::JMP:
        case Instructions::JE: case Instructions::JNE:
        case Instructions::JG: case Instructions::JGE:
        case Instructions::JL: case Instructions::JLE:
        case Instructions::JA: case Instructions::JAE:
        case Instructions::JB: case Instructions::JBE:
        case Instructions::JO: case Instructions::JNO:
        case Instructions::JS: case Instructions::JNS:
        case Instructions::JP: case Instructions::JNP:
        case Instructions::JC: case Instructions::JNC:
            return new x86::JMP_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::CALL:
            return new x86::CALL_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

        case Instructions::RET:
            return new x86::RET_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

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

        case Instructions::PUSH: case Instructions::POP:
            return new x86::Normal_Stack_Instruction(*this, instruction.bits, instruction.mnemonic, instruction.operands);

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

x86::Instruction::Instruction(::Encoder::Encoder& e, BitMode bits) : ::Encoder::Encoder::Instruction(e)
{
    bitmode = bits;
}

void x86::Instruction::encode(std::vector<uint8_t>& buffer)
{
    if (use16BitPrefix) buffer.push_back(prefix16Bit);
    if (use16BitAddressPrefix) buffer.push_back(addressPrefix16Bit);

    if (rex.use) buffer.push_back(getRex(rex.w, rex.r, rex.x, rex.b));

    if (useOpcodeEscape) buffer.push_back(opcodeEscape);
    buffer.push_back(opcode);

    if (modrm.use)
    {
        if (sib.use) buffer.push_back(getModRM(modrm.mod, modrm.reg, modRMSIB));
        else         buffer.push_back(getModRM(modrm.mod, modrm.reg, modrm.rm));
    }

    if (sib.use)
    {
        if (modrm.use) buffer.push_back(getSIB(sib.scale, sib.index, modrm.rm));
        else           throw Exception::InternalError("Can't use SIB without ModR/M", -1, -1);
    }

    if (displacement.use)
    {
        ::Encoder::RelocationSize relocationSize;
        // TODO: FIXME: Just make better
        if (displacement.is_short)
        {
            buffer.push_back(static_cast<uint8_t>(displacement.value));
            relocationSize = ::Encoder::RelocationSize::Bit8;
        }
        else
        {
            uint32_t sizeInBytes;

            if (addressMode == AddressMode::Bits16)
            {
                relocationSize = ::Encoder::RelocationSize::Bit16;
                sizeInBytes = 2;
            }
            else
            {
                relocationSize = ::Encoder::RelocationSize::Bit32;
                sizeInBytes = 4;
            }

            uint64_t oldSize = buffer.size();
            buffer.resize(oldSize + sizeInBytes);

            std::memcpy(buffer.data() + oldSize, &displacement.value, sizeInBytes);
        }

        if (displacement.needsRelocation)
        {
            uint64_t currentOffset = 0;
            if (use16BitPrefix) currentOffset++;
            if (use16BitAddressPrefix) currentOffset++;
            if (rex.use) currentOffset++;
            if (useOpcodeEscape) currentOffset++;
            currentOffset++; // Opcode
            if (modrm.use) currentOffset++;
            if (sib.use) currentOffset++;

            AddRelocation(
                currentOffset,
                displacement.value,
                true,
                displacement.relocationUsedSection,
                ::Encoder::RelocationType::Absolute,
                relocationSize,
                displacement.is_signed,
                displacement.relocationIsExtern
            );
        }
    }
    else if (modrm.use && modrm.mod == Mod::INDIRECT_DISP8)
    {
        buffer.push_back(static_cast<uint8_t>(0));
    }
    else if (modrm.use && modrm.mod == Mod::INDIRECT_DISP32)
    {
        buffer.push_back(static_cast<uint8_t>(0));
        buffer.push_back(static_cast<uint8_t>(0));
        if (addressMode != AddressMode::Bits16)
        {
            buffer.push_back(static_cast<uint8_t>(0));
            buffer.push_back(static_cast<uint8_t>(0));
        }
    }

    encodeS(buffer);
}

uint64_t x86::Instruction::size()
{
    uint64_t s = 0;

    if (use16BitPrefix) s++;
    if (use16BitAddressPrefix) s++;

    if (rex.use) s++;

    if (useOpcodeEscape) s++;
    s++; // Opcode

    if (modrm.use) s++;
    if (sib.use) s++;

    if (displacement.use)
    {
        if (displacement.is_short) s++;
        else
        {
            if (addressMode == AddressMode::Bits16) s += 2;
            else                                    s += 4;
        }
    }
    else if (modrm.use && modrm.mod == Mod::INDIRECT_DISP8)
    {
        s++;
    }
    else if (modrm.use && modrm.mod == Mod::INDIRECT_DISP32)
    {
        if (addressMode == AddressMode::Bits16) s += 2;
        else                                    s += 4;
    }

    s += sizeS();

    return s;
}

void x86::Instruction::evaluate()
{
    evaluateDisplacement();
    evaluateS();
}

bool x86::Instruction::optimize()
{
    bool changed = false;

    if (optimizeDisplacement())
        changed = true;

    if (optimizeS())
        changed = true;

    return changed;
}

void x86::Instruction::evaluateDisplacement()
{
    // TODO: RIP-Relative

    if (displacement.use)
    {
        ::Encoder::Evaluation evaluation = Evaluate(displacement.immediate, false, 0);

        if (evaluation.useOffset)
        {
            displacement.needsRelocation = true;

            displacement.relocationUsedSection = evaluation.usedSection;
            displacement.relocationIsExtern = evaluation.isExtern;
            
            displacement.can_optimize = false;
            displacement.value = evaluation.offset;
        }
        else
        {
            Int128 result = evaluation.result;

            // TODO: Check for overflow

            displacement.value = static_cast<uint64_t>(result);
        }
    }
}

bool x86::Instruction::optimizeDisplacement()
{
    if (displacement.use && displacement.can_optimize && !displacement.is_short)
    {
        if (displacement.value < -128 || displacement.value > 127)
            return false;

        displacement.is_short = true;

        // TODO: Make better
        modrm.mod = Mod::INDIRECT_DISP8;

        return true;
    }

    return false;
}

void x86::Instruction::checkReg(const Parser::Instruction::Register& reg, BitMode bits)
{
    if (bits == BitMode::Bits64) return;
    switch (reg.reg)
    {
        case SPL: case BPL: case SIL: case DIL:
        case R8B: case R9B: case R10B: case R11B:
        case R12B: case R13B: case R14B: case R15B:
        case R8W: case R9W: case R10W: case R11W:
        case R12W: case R13W: case R14W: case R15W:
        case R8D: case R9D: case R10D: case R11D:
        case R12D: case R13D: case R14D: case R15D:
        case RAX: case RCX: case RDX: case RBX:
        case RSP: case RBP: case RSI: case RDI:
        case R8: case R9: case R10: case R11:
        case R12: case R13: case R14: case R15:

        case CR8: case CR9: case CR10:
        case CR11: case CR12: case CR13:
        case CR14: case CR15:

        case DR8: case DR9: case DR10:
        case DR11: case DR12: case DR13:
        case DR14: case DR15:
            throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
    }
}

void x86::Instruction::checkSize(uint64_t size, BitMode bits)
{
    if (bits == BitMode::Bits64) return;
    if (size == 64)
        throw Exception::SyntaxError("64-bit size not supported in 16/32 bit mode", -1, -1);
}
