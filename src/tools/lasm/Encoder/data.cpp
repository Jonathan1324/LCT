#include "data.hpp"

Encoder::Data::Data_Instruction::Data_Instruction(Encoder& e, const Parser::DataDefinition& dataDefinition)
    : Encoder::Instruction(e)
{
    valueSize = dataDefinition.size;
    values = dataDefinition.values;
    bufferSize = valueSize * values.size();

    switch (valueSize)
    {
        case 1: case 2: case 4: case 8:
            break;

        default:
            throw Exception::SyntaxError("Only initialized data with a length of 1, 2, 4 or 8 bytes can be used.", -1, -1);
    }

    for (const Parser::Immediate& value : values)
    {
        if (value.operands.empty())
            throw Exception::SemanticError("Data definition cannot be empty", dataDefinition.lineNumber, dataDefinition.column);
    }
}

void Encoder::Data::Data_Instruction::evaluate()
{
    iBuffer.clear();

    uint64_t offset = 0;
    for (const Parser::Immediate& value : values)
    {
        Evaluation evaluation = Evaluate(value, false, 0);

        if (evaluation.useOffset)
        {
            for (size_t i = 0; i < valueSize; i++)
            {
                uint8_t byte = static_cast<uint8_t>((evaluation.offset >> (i * 8)) & 0xFF);
                iBuffer.push_back(byte);
            }

            RelocInfo relocInfo;
            relocInfo.relocOffset = offset;
            relocInfo.offset = evaluation.offset;
            relocInfo.usedSection = evaluation.usedSection;
            relocInfo.isExtern = evaluation.isExtern;

            switch (valueSize)
            {
                case 1: relocInfo.size = ::Encoder::RelocationSize::Bit8; break;
                case 2: relocInfo.size = ::Encoder::RelocationSize::Bit16; break;
                case 3: relocInfo.size = ::Encoder::RelocationSize::Bit24; break;
                case 4: relocInfo.size = ::Encoder::RelocationSize::Bit32; break;
                case 8: relocInfo.size = ::Encoder::RelocationSize::Bit64; break;

                default: throw Exception::InternalError("Unknown size in bits " + std::to_string(valueSize), -1, -1);
            }

            relocs.push_back(std::move(relocInfo));
        }
        else
        {
            for (uint64_t i = 0; i < valueSize; i++)
            {
                uint8_t byte = static_cast<uint8_t>((evaluation.result >> (i * 8)) & 0xFF);
                iBuffer.push_back(byte);
            }
        }

        offset += valueSize;
    }
}

void Encoder::Data::Data_Instruction::encode(std::vector<uint8_t>& buffer)
{
    for (const RelocInfo& relocInfo : relocs)
    {
        AddRelocation(
            relocInfo.relocOffset,
            relocInfo.offset,
            true,
            relocInfo.usedSection,
            RelocationType::Absolute,
            relocInfo.size,
            false, // TODO: Check if signed
            relocInfo.isExtern
        );
    }

    buffer.insert(buffer.end(), iBuffer.begin(), iBuffer.end());
}

uint64_t Encoder::Data::Data_Instruction::size()
{
    return bufferSize;
}


Encoder::Data::ReservedData_Instruction::ReservedData_Instruction(Encoder& e, const Parser::DataDefinition& dataDefinition)
    : Encoder::Instruction(e)
{
    if (dataDefinition.values.size() != 1 )
        throw Exception::SyntaxError("Can only use resX with one argument", -1, -1);

    const Parser::Immediate& value = dataDefinition.values[0];

    iBuffer.clear();

    Evaluation evaluation = Evaluate(value, false, 0);

    if (evaluation.useOffset)
        throw Exception::SemanticError("Can't use relocations for resX", -1, -1);

    bufferSize = static_cast<uint64_t>(dataDefinition.size * evaluation.result);

    for (uint64_t i = 0; i < bufferSize; i++)
    {
        iBuffer.push_back(static_cast<uint8_t>(0));
    }
}

void Encoder::Data::ReservedData_Instruction::encode(std::vector<uint8_t>& buffer)
{
    buffer.insert(buffer.end(), iBuffer.begin(), iBuffer.end());
}

uint64_t Encoder::Data::ReservedData_Instruction::size()
{
    return bufferSize;
}
