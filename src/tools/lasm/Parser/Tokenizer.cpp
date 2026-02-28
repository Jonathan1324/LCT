#include "Tokenizer.hpp"

#include <Exception.hpp>
#include <util/string.hpp>
#include <cstdint>

Token::Tokenizer::Tokenizer(const Context& _context)
    : context(&_context)
{
	
}

void Token::Tokenizer::clear() {
    tokens.clear();
}

void Token::Tokenizer::tokenize(std::istream* input)
{
    StringPool::String file = context->stringPool->GetString("-"); // TODO
    std::string line;
    size_t lineNumber = 0;
    size_t lineIncrease = 1;

    // TODO: Make dynamic
    const char commentStart = ';';

    while (std::getline(*input, line))
    {
        lineNumber += lineIncrease;
        size_t pos = 0;
        size_t length = line.size();

        std::string trimmed = trim(line);

        if (!trimmed.empty() && trimmed[0] == commentStart)
        {
            continue;
        }

        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos);
            length = line.size();
        }

        if (trimmed.find("%line") == 0)
        {
            std::string rest = trim(trimmed.substr(5));
            size_t plusPos = rest.find('+');
            size_t spacePos = rest.find(' ');

            lineNumber = std::stoul(trim(rest.substr(0, plusPos))) - 1;

            if (plusPos != std::string::npos) {
                lineIncrease = std::stoul(trim(rest.substr(plusPos + 1, spacePos - plusPos - 1)));
            }

            if (spacePos != std::string::npos) {
                std::string filename = trim(rest.substr(spacePos + 1));
                if (filename == "-") {
                    filename = context->filename;
                }
                file = context->stringPool->GetString(filename);
            }

            // TODO: parse (including when seeing '-' as filename to put the main file there)
            continue;
        }

        while (pos < length)
        {
            // Skip whitespace
            while (pos < length && std::isspace(static_cast<unsigned char>(line[pos])))
                pos++;
            if (pos >= length) break;

            size_t startPos = pos;

            // ,
            if (line[pos] == ',')
            {
                tokens.emplace_back(
                    Type::Comma,
                    context->stringPool->GetString(","),
                    lineNumber,
                    pos + 1,
                    file
                );
                pos++;
            }
            // ; or :
            else if (line[pos] == ';' || line[pos] == ':')
            {
                tokens.emplace_back(Type::Punctuation, context->stringPool->GetString(line[pos]), lineNumber, pos, file);
                pos++;
            }

            // +,-,*,/
            else if (line[pos] == '+' || line[pos] == '-' || line[pos] == '*' || line[pos] == '/' || line[pos] == '%')
            {
                tokens.emplace_back(Type::Operator, context->stringPool->GetString(line[pos]), lineNumber, pos, file);
                pos++;
            }

            // Bracket
            else if (line[pos] == '(' || line[pos] == ')' ||
                     line[pos] == '[' || line[pos] == ']' ||
                     line[pos] == '{' || line[pos] == '}')
            {
                tokens.emplace_back(
                    Type::Bracket,
                    context->stringPool->GetString(line[pos]),
                    lineNumber,
                    pos + 1,
                    file
                );
                pos++;
            }

            // Strings
            else if (line[pos] == '"')
            {
                pos++;  // skip opening "
                startPos = pos;
                std::string value;
                while (pos < length)
                {
                    if (line[pos] == '\\')
                    {
                        pos++;
                        switch(line[pos])
                        {
                            case '\\': value.push_back('\\'); pos++; break;
                            case '"': value.push_back('"'); pos++; break;
                            case '\'': value = '\''; pos++; break;
                            case 'n': value = '\n'; pos++; break;
                            // TODO: add more

                            default: throw Exception::SyntaxError("Unknown escape character", lineNumber, pos);
                        }
                    }
                    else if (line[pos] == '"')
                    {
                        break;
                    }
                    else
                    {
                        value.push_back(line[pos]);
                        pos++;
                    }
                }

                tokens.emplace_back(Type::String, context->stringPool->GetString(value), lineNumber, startPos, file);

                if (pos < length && line[pos] == '"')
                    pos++; // skip closing "
            }
            // Characters
            else if (line[pos] == '\'')
            {
                pos++;  // skip opening '
                startPos = pos;
                char value;
                if (line[pos] == '\\')
                {
                    pos++;
                    if (pos >= line.length())
                        throw Exception::SyntaxError("Unexpected end of line after escape character", lineNumber, pos);

                    // TODO: one function only
                    switch(line[pos])
                    {
                        case '\\': value = '\\'; break;
                        case '"': value = '"'; break;
                        case '\'': value = '\''; break;
                        case 'n': value = '\n';break;
                        // TODO: add more

                        default: throw Exception::SyntaxError("Unknown escape character", lineNumber, pos);
                    }
                }
                else
                {
                    value = line[pos];
                }

                pos++;

                if (pos >= line.length() || line[pos] != '\'')
                {
                    throw Exception::SyntaxError("Expected closing '", lineNumber, pos);
                }

                tokens.emplace_back(Type::Character, context->stringPool->GetString(value), lineNumber, startPos, file);

                pos++; // skip closing '
            }

            // Everything else
            else
            {
                while (pos < length &&
                       !std::isspace(static_cast<unsigned char>(line[pos])) &&
                       line[pos] != ',' && line[pos] != ';' && line[pos] != ':' &&
                       line[pos] != '(' && line[pos] != ')' && line[pos] != '[' &&
                       line[pos] != ']' && line[pos] != '{' && line[pos] != '}' &&
                       line[pos] != '"' && line[pos] != '\'' &&
                       line[pos] != '+' && line[pos] != '-' && line[pos] != '*' && line[pos] != '/' && line[pos] != '%')
                    pos++;
                
                tokens.emplace_back(
                    Type::Token,
                    context->stringPool->GetString(line.substr(startPos, pos - startPos)) ,
                    lineNumber,
                    startPos + 1,
                    file
                );
            }
        }

        tokens.emplace_back(
            Type::EOL,
            context->stringPool->empty(),
            lineNumber,
            length + 1,
            file
        );
    }
    tokens.emplace_back(
        Type::_EOF,
        context->stringPool->empty(),
        lineNumber + 1,
        0,
        file
    );
}

std::vector<Token::Token> Token::Tokenizer::getTokens()
{
    return tokens;
}

void Token::Tokenizer::print()
{
    std::cout << "Tokens: " << std::endl;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token& token = tokens[i];
        std::cout << "  " << token.what(context) << std::endl;
    }
}

std::string Token::Token::what(const Context* context) const
{
    (void)context; // TODO

    std::string result = std::string("Token (Type=") + to_string(type) + ")";

    switch (type)
    {
        case Type::Comma:
        case Type::EOL:
        case Type::_EOF:
            result += " ";
            break;

        case Type::Character:
        {
            unsigned char c = static_cast<unsigned char>(value[0]);
            result += std::to_string(c) + " ";
            break;
        }

        case Type::Token:
        case Type::String:
        case Type::Bracket:
        case Type::Punctuation:
        default:
            result += std::string(" '") + value.c_str() + "' ";
            break;
    }

    result += "in line " + std::to_string(line) + " at column " + std::to_string(column) + " in file " + file.c_str();

    return result;
}
