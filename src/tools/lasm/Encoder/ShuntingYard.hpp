#pragma once

#include <IntTypesC.h>
#include <string>
#include "Encoder.hpp"

namespace ShuntingYard
{
    struct Token
    {
        enum class Type { Number, Operator, Position };
        Type type;

        Int128 number;
        StringPool::String op;
        uint64_t offset;
        bool negative = false;

        Token(Int128 n) : type(Type::Number), number(n) {}
        Token(const StringPool::String& o) : type(Type::Operator), op(o) {}
        Token() {}
    };

    struct PreparedTokens {
        std::vector<Token> tokens;
        bool relocationPossible;
        StringPool::String usedSection;
        bool useSection = false;
        bool isExtern = false;
    };

    PreparedTokens prepareTokens(
        const std::vector<Parser::ImmediateOperand>& operands,
        std::unordered_map<std::string, Encoder::Label>& labels,
        const std::unordered_map<std::string, Encoder::Constant>& constants,
        uint64_t sectionOffset,
        StringPool::String currentSection
    );

    Int128 evaluate(const std::vector<Token>& tokens, uint64_t offset);
}