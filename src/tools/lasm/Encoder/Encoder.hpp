#pragma once

#include <Architecture.hpp>
#include <vector>
#include <IntTypesC.h>
#include <unordered_set>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"

namespace Encoder
{
    using SectionBuffer = std::vector<uint8_t>;

    struct Label
    {
        std::string name;
        std::string section;
        uint64_t offset = 0;

        bool isGlobal;
        bool resolved;
        bool isExtern = false;
        bool externUsed = false;
    };

    enum class HasPos
    {
        UNKNOWN,
        TRUE,
        FALSE
    };

    struct Constant
    {
        std::string name;
        std::string section;
        Parser::Immediate expression;
        int64_t value;

        int64_t off;
        std::string usedSection;

        HasPos hasPos;
        bool useOffset = false;

        size_t offset;
        size_t bytesWritten;

        bool isGlobal;
        bool resolved;
        bool prePass = false;
        bool relocationPossible = false;
    };

    enum class RelocationType
    {
        Absolute
    };

    enum class RelocationSize
    {
        Bit8 = 8,
        Bit16 = 16,
        Bit24 = 24,
        Bit32 = 32,
        Bit64 = 64
    };

    struct Relocation
    {
        uint64_t offsetInSection;
        int64_t addend;
        std::string section;
        std::string usedSection;

        RelocationType type;
        RelocationSize size;

        bool addendInCode = false;
        bool isExtern = false;

        bool isSigned = false;
    };

    struct Evaluation
    {
        Int128 result;
        int64_t offset;
        bool useOffset;
        bool relocationPossible;
        bool isExtern;

        std::string usedSection;
    };

    class Encoder
    {
    public:
        class Instruction
        {
            friend class Encoder;

        protected:
            Encoder& enc;

            Instruction(Encoder& e) : enc(e) {}

            inline BitMode getBitMode() { return enc.bits; }

            inline ::Encoder::Evaluation Evaluate(
                const Parser::Immediate& immediate,
                bool ripRelative, uint64_t ripExtra
            )
            { return enc.Evaluate(immediate, enc.bytesWritten, enc.sectionOffset, enc.currentSection, ripRelative, ripExtra); }

            void AddRelocation(
                uint64_t extra_offset, uint64_t addend,
                bool addendInCode, const std::string& usedSection,
                ::Encoder::RelocationType type,
                ::Encoder::RelocationSize size,
                bool isSigned,
                bool isExtern
            )
            {
                ::Encoder::Relocation relocation;
                relocation.offsetInSection = enc.sectionOffset + extra_offset;
                relocation.addend = addend;
                relocation.addendInCode = addendInCode;
                relocation.section = *enc.currentSection;
                relocation.usedSection = usedSection;
                relocation.type = type;
                relocation.size = size;
                relocation.isSigned = isSigned;
                relocation.isExtern = isExtern;
                enc.relocations.push_back(std::move(relocation));
            }

        public:
            virtual ~Instruction() {};

            virtual std::vector<uint8_t> encode() = 0;
            virtual uint64_t size() = 0;
            virtual void evaluate() = 0;
            virtual bool optimize() = 0;
        };

        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        virtual ~Encoder() = default;

        void Encode();
        void Optimize();
        void Print() const;

        using Symbol = std::variant<Label*, Constant*>;
        const std::vector<struct Section>& getSections() const { return sections; };
        const std::vector<Symbol>& getSymbols() const { return symbols; };
        const std::vector<Relocation>& getRelocations() const { return relocations; }
        
    protected:
        void Initialize();

        void EvaluationPhase();
        void CalculationPhase();
        bool OptimizationPhase();

        void GenerateCode();

        Instruction* GetDataDefinition(const Parser::DataDefinition& dataDefinition);

        virtual Instruction* GetInstruction(const Parser::Instruction::Instruction& instruction) = 0;

        virtual std::vector<uint8_t> EncodePadding(size_t length) = 0;

        Evaluation Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset, const std::string* curSection, bool ripRelative, uint64_t ripExtra);

        void resolveConstants(bool withPos);
        bool Resolvable(const Parser::Immediate& immediate);
        std::vector<std::string> getDependencies(const Parser::Immediate& immediate);
        bool resolveConstantWithoutPos(Constant& c, std::unordered_set<std::string>& visited);
        bool resolveConstantWithPos(Constant& c, std::unordered_set<std::string>& visited);

        Context context;
        Architecture arch;
        BitMode bits;

        const Parser::Parser* parser = nullptr;

        std::vector<struct Section> sections;
        std::vector<Relocation> relocations;

        std::unordered_map<std::string, uint64_t> sectionStarts;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Constant> constants;

        std::vector<Symbol> symbols;

        size_t bytesWritten = 0;
        size_t sectionOffset = 0;
        const std::string* currentSection;
    };

    class SectionEntry
    {
    public:
        struct Label
        {
            Label(std::string _name) : name(_name) {}

            std::string name;
        };

        struct Constant
        {
            Constant(std::string _name) : name(_name) {}

            std::string name;
        };

        SectionEntry(Encoder::Instruction* instr)
        {
            entry = instr;
        }

        bool isInstruction() const
        {
            return std::holds_alternative<Encoder::Instruction*>(entry);
        }

        Encoder::Instruction* getInstruction() const
        {
            if (!isInstruction()) throw Exception::InternalError("Entry not an instruction", -1, -1);
            return std::get<Encoder::Instruction*>(entry);
        }


        SectionEntry(const Label& label)
        {
            entry = label;
        }

        bool isLabel() const
        {
            return std::holds_alternative<Label>(entry);
        }

        Label getLabel() const
        {
            if (!isLabel()) throw Exception::InternalError("Entry not a label", -1, -1);
            return std::get<Label>(entry);
        }


        SectionEntry(const Constant& constant)
        {
            entry = constant;
        }

        bool isConstant() const
        {
            return std::holds_alternative<Constant>(entry);
        }

        Constant getConstant() const
        {
            if (!isConstant()) throw Exception::InternalError("Entry not a constant", -1, -1);
            return std::get<Constant>(entry);
        }

    private:
        std::variant<Encoder::Instruction*, Label, Constant> entry;
    };

    struct Section
    {
        std::string name;
        bool isInitialized = true;
        SectionBuffer buffer;
        size_t reservedSize = 0;

        uint64_t align;

        std::vector<SectionEntry> instructions;

        size_t size() const;
    };

    Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser);
}
