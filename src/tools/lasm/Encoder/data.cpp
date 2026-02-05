#include "data.hpp"

Encoder::Data::Data_Instruction::Data_Instruction(Encoder& e, const Parser::DataDefinition& dataDefinition)
    : Encoder::Instruction(e)
{
    if (dataDefinition.reserved)
    {
        // TODO
        throw Exception::InternalError("Reserved data encoding is not implemented yet :(", dataDefinition.lineNumber, dataDefinition.column);
    }
    else
    {
        valueSize = dataDefinition.size;
        values = dataDefinition.values;
        bufferSize = valueSize * values.size();

        for (const Parser::Immediate& value : values)
        {
            if (value.operands.empty())
                throw Exception::SemanticError("Data definition cannot be empty", dataDefinition.lineNumber, dataDefinition.column);
        }
    }
}

void Encoder::Data::Data_Instruction::evaluate()
{
    buffer.clear();

    for (const Parser::Immediate& value : values)
    {
        Evaluation evaluation = Evaluate(value);

        if (evaluation.useOffset)
        {
            for (size_t i = 0; i < valueSize; i++)
            {
                uint8_t byte = static_cast<uint8_t>((evaluation.offset >> (i * 8)) & 0xFF);
                buffer.push_back(byte);
            }

            ::Encoder::RelocationSize relocSize;
            switch (valueSize)
            {
                case 1: relocSize = ::Encoder::RelocationSize::Bit8; break;
                case 2: relocSize = ::Encoder::RelocationSize::Bit16; break;
                case 3: relocSize = ::Encoder::RelocationSize::Bit24; break;
                case 4: relocSize = ::Encoder::RelocationSize::Bit32; break;
                case 8: relocSize = ::Encoder::RelocationSize::Bit64; break;

                default: throw Exception::InternalError("Unknown size in bits " + std::to_string(valueSize), -1, -1);
            }

            AddRelocation(
                0,
                evaluation.offset,
                true,
                evaluation.usedSection,
                RelocationType::Absolute,
                relocSize,
                evaluation.isExtern
            );
        }
        else
        {
            for (uint64_t i = 0; i < valueSize; i++)
            {
                uint8_t byte = static_cast<uint8_t>((evaluation.result >> (i * 8)) & 0xFF);
                buffer.push_back(byte);
            }
        }
    }
}

std::vector<uint8_t> Encoder::Data::Data_Instruction::encode()
{
    return buffer;
}

uint64_t Encoder::Data::Data_Instruction::size()
{
    return bufferSize;
}
