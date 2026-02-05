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

            std::vector<uint8_t> encode() override;

            uint64_t size() override;

        private:
            uint64_t valueSize;
            std::vector<Parser::Immediate> values;

            uint64_t bufferSize;
            std::vector<uint8_t> buffer;
        };
    }
}
