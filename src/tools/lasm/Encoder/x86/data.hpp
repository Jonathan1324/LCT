#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include "x86.hpp"

namespace x86 {
    class Mov_Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        Mov_Instruction(::Encoder::Encoder& e, BitMode bits, uint64_t mnemonic, std::vector<Parser::Instruction::Operand> operands) : ::Encoder::Encoder::Instruction(e) {
            switch (mnemonic)
            {
                case Instructions::MOV: {
                    if (operands.size() != 2)
                        throw Exception::InternalError("Wrong argument count for 'mov'", -1, -1, nullptr);

                    destinationOperand = operands[0];
                    sourceOperand = operands[1];

                    if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
                    {
                        Parser::Instruction::Register destReg = std::get<Parser::Instruction::Register>(destinationOperand);

                        switch (destReg.reg)
                        {
                            case SPL: case BPL:
                            case SIL: case DIL:
                            case R8B: case R9B:
                            case R10B: case R11B:
                            case R12B: case R13B:
                            case R14B: case R15B:
                            case R8W: case R9W:
                            case R10W: case R11W:
                            case R12W: case R13W:
                            case R14W: case R15W:
                            case R8D: case R9D:
                            case R10D: case R11D:
                            case R12D: case R13D:
                            case R14D: case R15D:
                            case RAX: case RCX:
                            case RDX: case RBX:
                            case RSP: case RBP:
                            case RSI: case RDI:
                            case R8: case R9:
                            case R10: case R11:
                            case R12: case R13:
                            case R14: case R15:
                                if (bits != BitMode::Bits64)
                                    throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                        }

                        if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                        {
                            movType = MovType::MOV_REG_REG;

                            Parser::Instruction::Register srcReg = std::get<Parser::Instruction::Register>(sourceOperand);

                            switch (srcReg.reg)
                            {
                                case SPL: case BPL:
                                case SIL: case DIL:
                                case R8B: case R9B:
                                case R10B: case R11B:
                                case R12B: case R13B:
                                case R14B: case R15B:
                                case R8W: case R9W:
                                case R10W: case R11W:
                                case R12W: case R13W:
                                case R14W: case R15W:
                                case R8D: case R9D:
                                case R10D: case R11D:
                                case R12D: case R13D:
                                case R14D: case R15D: // TODO: Add more
                                    if (bits != BitMode::Bits64)
                                        throw Exception::SyntaxError("register only supported in 64-bit mode", -1, -1);
                            }

                            mod_mod = ::x86::Encoder::Mod::REGISTER;

                            // TODO
                        }
                        else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                        {
                            throw Exception::InternalError("Memory not supported yet with 'mov'", -1, -1);
                            // TODO
                        }
                        else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                        {
                            movType = MovType::MOV_REG_IMM;
                            // TODO
                        }
                    }
                    else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                    {
                        // TODO
                        throw Exception::InternalError("Memory not supported yet with 'mov'", -1, -1);

                        if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                        {
                            // TODO
                        }
                        else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                        {
                            // TODO
                        }
                        else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                        {
                            // TODO
                        }
                    }

                    break;
                }

                default:
                    throw Exception::InternalError("Unknown argument control instruction", -1, -1, nullptr);
            }
        }

        ~Mov_Instruction() override {

        }

        void evaluate() override {
            
        }

        std::vector<uint8_t> encode() override {
            std::vector<uint8_t> instr;

            if (use16BitPrefix) instr.push_back(prefix16Bit);

            if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));

            // TODO: logic
            if (useOpcodeEscape) instr.push_back(opcodeEscape);


            return instr;
        }

        uint64_t size() override {
            uint64_t s = 1;

            if (use16BitPrefix) s++;

            if (useREX) s++;

            if (useOpcodeEscape) s++;

            if (useModRM) s++;

            return s;
        }

    private:
        uint8_t opcode;

        Parser::Instruction::Operand destinationOperand;
        Parser::Instruction::Operand sourceOperand;

        enum class MovType {
            MOV_REG_REG,
            
            MOV_REG_IMM,
        } movType;

        bool use16BitPrefix = false;
        
        bool useREX = false;
        bool rexW = false;
        bool rexR = false;
        bool rexX = false;
        bool rexB = false;

        bool useOpcodeEscape = false;

        bool useModRM = false;
        ::x86::Encoder::Mod mod_mod;
        uint8_t mod_reg;
        uint8_t mod_rm;
    };
}
