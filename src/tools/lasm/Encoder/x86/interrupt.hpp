#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include "x86.hpp"

namespace x86 {
    class Argument_Interrupt_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Argument_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands) : ::Encoder::Encoder::Instruction(e) {
            switch (mnemonic)
            {
                case Instructions::INT: {
                    opcode = 0xCD;

                    if (operands.empty())
                        throw Exception::InternalError("'int' requires one argument", -1, -1, nullptr);
                    if (!std::holds_alternative<Parser::Immediate>(operands[0]))
                        throw Exception::InternalError("'int': wrong operand type", -1, -1, nullptr);

                    const Parser::Immediate& immediate = std::get<Parser::Immediate>(operands[0]);

                    // TODO

                    break;
                }

                default:
                    throw Exception::InternalError("Unknown argument control instruction", -1, -1, nullptr);
            }
        }

        ~Argument_Interrupt_Instruction() override {

        }

        void evaluate() override {
            ::Encoder::Evaluation evalutation = Evaluate(argument);

            if (evalutation.useOffset)
            {
                argument_value = evalutation.offset;

                AddRelocation(
                    1, // opcode
                    evalutation.offset, true,
                    evalutation.usedSection,
                    ::Encoder::RelocationType::Absolute,
                    ::Encoder::RelocationSize::Bit8,
                    evalutation.isExtern
                );
            }
            else
            {
                Int128 result = evalutation.result;

                if (result < 0)   throw Exception::SemanticError("'int' can't have a negative operand", -1, -1);
                if (result > 255) throw Exception::SemanticError("Operand too large for 'int'", -1, -1);

                argument_value = static_cast<uint8_t>(result);
            }
        }

        std::vector<uint8_t> encode() override {
            return {opcode, argument_value};
        }

        uint64_t size() override {
            return 2;
        }

    private:
        uint8_t opcode;

        Parser::Immediate argument;

        uint8_t argument_value;
    };

    class Simple_Interrupt_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Simple_Interrupt_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic) : ::Encoder::Encoder::Instruction(e) {
            switch (mnemonic)
            {
                case Instructions::IRET:
                    opcode = 0xCF;

                    break;

                case Instructions::IRETQ:
                    if (bits == BitMode::Bits16) throw Exception::SyntaxError("'iretq' not supported in 16-bit mode", -1, -1, nullptr);
                    if (bits == BitMode::Bits32) throw Exception::SyntaxError("'iretq' not supported in 32-bit mode", -1, -1, nullptr);

                    opcode = 0xCF;

                    useREX = true;
                    rexW = true;

                    break;

                case Instructions::IRETD:
                    if (bits == BitMode::Bits16) use16BitPrefix = true;

                    opcode = 0xCF;

                    break;

                case Instructions::SYSCALL:
                    opcode = 0x05;

                    useOpcodeEscape = true;

                    break;

                case Instructions::SYSRET:
                    opcode = 0x07;

                    useOpcodeEscape = true;

                    break;

                case Instructions::SYSENTER:
                    opcode = 0x34;

                    useOpcodeEscape = true;

                    break;

                case Instructions::SYSEXIT:
                    opcode = 0x35;

                    useOpcodeEscape = true;

                    break;

                default:
                    throw Exception::InternalError("Unknown simple control instruction", -1, -1, nullptr);
            }
        }

        ~Simple_Interrupt_Instruction() override {

        }

        void evaluate() override {}

        std::vector<uint8_t> encode() override {
            std::vector<uint8_t> instr;

            if (use16BitPrefix) instr.push_back(prefix16Bit);
            if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));

            if (useOpcodeEscape) instr.push_back(opcodeEscape);
            
            instr.push_back(opcode);

            return instr;
        }

        uint64_t size() override {
            uint64_t s = 1;

            if (useREX) s++;
            if (use16BitPrefix) s++;

            if (useOpcodeEscape) s++;

            return s;
        }

    private:
        uint8_t opcode;

        bool use16BitPrefix = false;

        bool useREX = false;
        bool rexW = false;
        bool rexR = false;
        bool rexX = false;
        bool rexB = false;

        bool useOpcodeEscape = false;
    };
}
