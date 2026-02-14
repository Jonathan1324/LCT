#pragma once

#include "Encoder.hpp"

namespace Encoder
{
    namespace Data
    {
        class Data_Instruction : public Encoder::Instruction
        {
        public:
            Data_Instruction(Encoder& e, const Parser::DataDefinition& dataDefinition);
            ~Data_Instruction() override {}

            void evaluate() override;
            bool optimize() override { return false; }
            void encode(std::vector<uint8_t>& buffer) override;
            uint64_t size() override;

        private:
            uint64_t valueSize;
            std::vector<Parser::Immediate> values;

            uint64_t bufferSize;
            std::vector<uint8_t> iBuffer;

            struct RelocInfo {
                uint64_t relocOffset;
                int64_t offset;
                std::string usedSection;
                bool isExtern;
                ::Encoder::RelocationSize size;
            };

            std::vector<RelocInfo> relocs;
        };

        class ReservedData_Instruction : public Encoder::Instruction
        {
        public:
            ReservedData_Instruction(Encoder& e, const Parser::DataDefinition& dataDefinition);
            ~ReservedData_Instruction() override {}

            void evaluate() override {}
            bool optimize() override { return false; }
            void encode(std::vector<uint8_t>& buffer) override;
            uint64_t size() override;

        private:
            uint64_t bufferSize;
            std::vector<uint8_t> iBuffer;
        };
    }
}
