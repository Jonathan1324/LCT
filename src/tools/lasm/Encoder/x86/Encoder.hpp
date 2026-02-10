#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include <tuple>
#include "x86.hpp"

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

    class Instruction : public ::Encoder::Encoder::Instruction
    {
    public:
        virtual ~Instruction() {};

        std::vector<uint8_t> encode() override;

    protected:
        virtual void encodeS(std::vector<uint8_t>& buffer) = 0;

        Instruction(::Encoder::Encoder& e) : ::Encoder::Encoder::Instruction(e) {}

        uint8_t opcode;
        bool useOpcodeEscape = false;

        bool use16BitPrefix = false;
        bool use16BitAddressPrefix = false;

        struct REX {
            bool use = false;

            bool w = false;
            bool r = false;
            bool x = false;
            bool b = false;
        } rex;

        struct ModRM {
            bool use = false;

            ::x86::Mod mod;
            uint8_t reg;
            uint8_t rm;
        } modrm;

        struct SIB {
            bool use = false;

            uint8_t index;
            Scale scale;
        } sib;
    };
}
