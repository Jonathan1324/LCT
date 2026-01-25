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
                    else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
                    {
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
            
        }

        uint64_t size() override {
            return 2;
        }

    private:
        uint8_t opcode;

        Parser::Instruction::Operand destinationOperand;
        Parser::Instruction::Operand sourceOperand;
    };
}
