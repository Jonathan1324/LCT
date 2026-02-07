#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include <tuple>

#include "control.hpp"
#include "interrupt.hpp"
#include "flag.hpp"
#include "stack.hpp"
#include "data.hpp"
#include "alu.hpp"

namespace x86
{
    class Encoder : public ::Encoder::Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        ~Encoder() = default;

    protected:
        ::Encoder::Encoder::Instruction* GetInstruction(const Parser::Instruction::Instruction& instruction) override;

        std::vector<uint8_t> EncodePadding(size_t length) override;
    };
}
