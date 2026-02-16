#pragma once

#include "../Encoder.hpp"
#include <x86/Instructions.hpp>
#include <tuple>
#include "x86.hpp"
#include <IntTypesC.h>

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

        void evaluate() override final;
        bool optimize() override final;
        void encode(std::vector<uint8_t>& buffer) override final;
        uint64_t size() override final;

    protected:
        virtual void evaluateS() {}
        virtual bool optimizeS() { return false; }
        virtual void encodeS(std::vector<uint8_t>& buffer) {};
        virtual uint64_t sizeS() { return 0; };

        void checkSize(uint64_t size, BitMode bits);
        
        uint64_t parseMemory(
            const Parser::Instruction::Memory& mem,
            BitMode bits,
            bool expectSize
        );

        uint64_t parseRegister(
            const Parser::Instruction::Register& reg,
            BitMode bits,
            bool isReg
        );

        inline uint8_t getRegIndex(const Parser::Instruction::Register& reg)
        {
            return std::get<0>(getReg(reg.reg));
        }

        void checkReg(const Parser::Instruction::Register& reg, BitMode bits);

        Instruction(::Encoder::Encoder& e, const ::Parser::Instruction::Instruction& instr);

        BitMode bits;

        bool use16BitPrefix = false;
        bool use16BitAddressPrefix = false;

        enum class AddressMode {
            Bits16,
            Bits32,
            Bits64
        } addressMode;

        struct REX {
            bool use = false;

            bool w = false;
            bool r = false;
            bool x = false;
            bool b = false;
        } rex;

        bool useOpcodeEscape = false;
        uint8_t opcode;

        struct ModRM {
            bool use = false;

            Mod mod = Mod::REGISTER;
            uint8_t reg = 0;
            uint8_t rm = 0;
        } modrm;

        struct SIB {
            bool use = false;

            uint8_t index;
            Scale scale = Scale::x1;
        } sib;

        struct Displacement {
            bool use = false;
            bool is_signed = false;
            bool is_short = false;

            bool can_optimize = true;
            bool direct_addressing = false;

            int64_t value;
            Parser::Immediate immediate;

            bool needsRelocation = false;
            StringPool::String relocationUsedSection;
            bool relocationIsExtern;
        } displacement;

        struct Immediate {
            bool use = false;
            bool is_signed = false;

            uint16_t sizeInBits;

            uint64_t value;
            Parser::Immediate immediate;

            bool needsRelocation = false;
            StringPool::String relocationUsedSection;
            bool relocationIsExtern;
        } immediate;
    };
}
