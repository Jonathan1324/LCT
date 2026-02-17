#include "Encoder.hpp"

#include "ShuntingYard.hpp"

Encoder::Evaluation Encoder::Encoder::Evaluate(const Parser::Immediate& immediate, uint64_t bWritten, uint64_t secOffset, StringPool::String curSection, bool ripRelative, uint64_t ripExtra)
{
    // TODO: ripRelative and ripExtra

    // substitute position with two different values:
    // if both are equal:                               position doesn't matter
    // if both are equal when subtracting position:     can be written using offset + position (relocation)
    // else:                                            not even relocation is possible
    ShuntingYard::PreparedTokens tokens = ShuntingYard::prepareTokens(immediate.operands, labels, constants, secOffset, curSection);

    if (tokens.relocationPossible)
    {
        uint64_t off1 = 348234582348;
        if (tokens.useSection && !tokens.isExtern)
        {
            auto it = sectionStarts.find(tokens.usedSection.c_str());
            if (it == sectionStarts.end()) throw Exception::InternalError("Couldn't find start of used section", -1, -1);
            off1 = it->second;
        }
        uint64_t off2 = off1 + 0x12345678;

        Int128 res1 = ShuntingYard::evaluate(tokens.tokens, off1);
        Int128 res2 = ShuntingYard::evaluate(tokens.tokens, off2);

        Evaluation evaluation;
        evaluation.usedSection = tokens.usedSection;
        evaluation.isExtern = tokens.isExtern;

        if (ripRelative)
        {
            uint64_t ripBase1 = bWritten + ripExtra;
            uint64_t ripBase2 = bWritten + ripExtra + (off2 - off1);
            
            res1 = res1 - static_cast<Int128>(ripBase1);
            res2 = res2 - static_cast<Int128>(ripBase2);

            evaluation.result = res1;

            if (res1 == res2 && tokens.useSection && currentSection == evaluation.usedSection)
            {
                evaluation.useOffset = false;
                evaluation.relocationPossible = true;
                evaluation.offset = 0;
            }
            else if (!tokens.useSection)
            {
                evaluation.useOffset = false;
                evaluation.relocationPossible = true;

                evaluation.result = res1 + ripBase1 - secOffset - ripExtra;
            }
            else // TODO: Check
            {
                evaluation.useOffset = true;
                evaluation.relocationPossible = true;
                evaluation.offset = 0;
            }
        }
        else
        {
            evaluation.result = res1;

            if (res1 == res2)
            {
                evaluation.useOffset = false;
                evaluation.relocationPossible = true;
                evaluation.offset = 0;
            }
            else if ((res1 - off1) == (res2 - off2))
            {
                evaluation.useOffset = true;
                evaluation.relocationPossible = true;
                evaluation.offset = static_cast<int64_t>(res1 - off1);
            }
            else
            {
                evaluation.useOffset = false;
                evaluation.relocationPossible = false;
                evaluation.offset = 0;
            }
        }

        if (evaluation.relocationPossible && evaluation.isExtern && tokens.useSection)
        {
            if (auto it = labels.find(evaluation.usedSection.c_str()); it != labels.end())
            {
                it->second.externUsed = true;
            }
        }

        return evaluation;
    }
    else
    {
        Int128 result = ShuntingYard::evaluate(tokens.tokens, bWritten - secOffset);

        Evaluation evaluation;
        evaluation.result = result;
        evaluation.usedSection = tokens.usedSection;
        evaluation.useOffset = false;
        evaluation.relocationPossible = false;
        evaluation.isExtern = false;
        evaluation.offset = 0;
        return evaluation;
    }
}
