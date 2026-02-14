#include "Parser.hpp"

#include <util/string.hpp>
#include <unordered_set>
#include <array>
#include <algorithm>
#include <x86/Registers.hpp>
#include <x86/Instructions.hpp>
#include "../evaluate.hpp"

x86::Parser::Parser(const Context& _context, Architecture _arch, BitMode _bits)
    : ::Parser::Parser(_context, _arch, _bits)
{

}

Parser::ImmediateOperand getOperand(const Token::Token& token, const std::string& lastMainLabel)
{
    if (token.type == Token::Type::Operator || token.type == Token::Type::Bracket)
    {
        Parser::Operator op;
        op.op = token.value;
        return op;
    }
    else if (token.type == Token::Type::Character)
    {
        Parser::Integer integer;
        integer.value = static_cast<uint64_t>(static_cast<unsigned char>(token.value[0]));
        return integer;
    }
    else if (token.type == Token::Type::Token && (token.value == "$" ||token.value == "$$"))
    {
        Parser::CurrentPosition curPos;
        curPos.sectionPos = (token.value == "$") ? false : true;
        return curPos;
    }
    else if (std::isdigit(static_cast<unsigned char>(token.value[0])) != 0)
    {
        Parser::Integer integer;
        // TODO: currently only integer
        integer.value = evalInteger(token.value, 8, token.line, token.column);
        return integer;
    }
    else
    {
        Parser::String str;

        if (token.value.size() > 0 && token.value[0] == '.')
            str.value = lastMainLabel + token.value;
        else
            str.value = token.value;

        return str;
    }
}

void x86::Parser::Parse(const std::vector<Token::Token>& tokens)
{
    std::vector<Token::Token> filteredTokens;
    Token::Type before = Token::Type::_EOF;

    std::string lastMainLabel = "";

    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token::Token& token = tokens[i];

        if (token.type == Token::Type::EOL && before == Token::Type::EOL)
            continue;

        before = token.type;
        filteredTokens.push_back(token);
    }

    std::vector<std::string> globals;

    for (auto it = filteredTokens.begin(); it != filteredTokens.end(); /* manual increment */)
    {
        bool hasOpeningBracket = false;
        bool hasClosingBracket = false;

        if (it->type == Token::Type::Bracket && it->value == "[")
        {
            auto next = std::next(it);
            if (next != filteredTokens.end())
            {
                const std::string val = toLower(next->value);
                if (val == "global" || val == "extern")
                {
                    hasOpeningBracket = true;
                    it = filteredTokens.erase(it);
                }
            }
        }

        const std::string lowerVal = toLower(it->value);

        if (lowerVal == "global" || lowerVal == "extern")
        {
            // Get the next token (the symbol name)
            auto next = std::next(it);
            if (next != filteredTokens.end())
            {
                if (lowerVal == "global")
                    globals.push_back(next->value);
                else
                {
                    Token::Token externToken(Token::Type::ExternLabel, next->value, next->line, next->column, next->file);
                    it = filteredTokens.insert(it, externToken);
                    it++;
                }
            }

            // remove "global"/"extern"
            it = filteredTokens.erase(it);

            // remove symbol
            if (it != filteredTokens.end())
                it = filteredTokens.erase(it);

            // remove ']' if it started with '['
            if (hasOpeningBracket)
            {
                if (it == filteredTokens.end() || it->type != Token::Type::Bracket || it->value != "]")
                    throw Exception::SyntaxError("Missing closing ']' after '[global ...' or '[extern ...'", it->line, it->column);
                it = filteredTokens.erase(it);
            }
            else
            {
                /* TODO: think about it
                if (it != filteredTokens.end() && it->type == Token::Type::Bracket && it->value == "]")
                    throw Exception::SyntaxError("Unexpected closing ']' after directive", it->line, it->column);
                */
            }

            // Erase tokens until end-of-line
            while (it != filteredTokens.end() && it->type != Token::Type::EOL)
            {
                it = filteredTokens.erase(it);  // erase returns the next valid iterator
            }

            // Erase the EOL token as well, if present
            if (it != filteredTokens.end())
                it = filteredTokens.erase(it);

            continue; // Don't increment, already done via erase
        }

        ++it; // only increment if nothing was erased
    }

    static constexpr std::array<std::string_view, 16> dataDefinitions = {
        "db", "dw", "dd", "dq", "dt", "do", "dy", "dz",
        "resb", "resw", "resd", "resq", "rest", "reso", "resy", "resz"
    };

    static constexpr std::array<std::string_view, 5> directives = {
        "section", "segment", "bits", "org", "align"
    };

    ::Parser::Section implicitSection;

    ::Parser::Section* currentSection = &implicitSection;
    BitMode currentBitMode = bits;

    for (size_t i = 0; i < filteredTokens.size(); i++)
    {
        const Token::Token& token = filteredTokens[i];
        if (token.type == Token::Type::EOL || token.type == Token::Type::_EOF)
            continue;
        
        const std::string lowerVal = toLower(token.value);

        // Constants
        if (filteredTokens[i + 1].type == Token::Type::Token && filteredTokens[i + 1].value.compare("equ") == 0)
        {
            ::Parser::Constant constant;
            constant.lineNumber = token.line;
            constant.column = token.column;
            // TODO: case sensitive
            constant.name = token.value;
            constant.hasPos = false;
            i += 2;

            if (std::find(globals.begin(), globals.end(), token.value) != globals.end())
                constant.isGlobal = true;
            else
                constant.isGlobal = false;

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                        if (std::holds_alternative<::Parser::CurrentPosition>(op) && !constant.hasPos)
                            constant.hasPos = true;
                        constant.value.operands.push_back(op);
                        i++;
                    }
                    i--;
                }
                else if (filteredTokens[i].type == Token::Type::String)
                {
                    // TODO
                }
                else
                    throw Exception::SyntaxError("Expected definition after 'equ'", filteredTokens[i].line, filteredTokens[i].column);

                i++;
            }

            currentSection->entries.push_back(constant);

            continue;
        }

        // times
        if (token.type == Token::Type::Token && lowerVal.compare("times") == 0)
        {
            ::Parser::Repetition repetition;
            repetition.lineNumber = token.line;
            repetition.column = token.column;

            i++;
            
            // TODO: strange way
            while (i < filteredTokens.size())
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                    repetition.count.operands.push_back(op);
                }
                else
                    throw Exception::SyntaxError("Unknown value type after 'times'", filteredTokens[i].line, filteredTokens[i].column);
                
                i++;
                if ( i < filteredTokens.size() && filteredTokens[i].type == Token::Type::EOL)
                    break;
            }
            i--;

            currentSection->entries.push_back(repetition);
            continue;
        }

        // Directives
        if ((token.type == Token::Type::Bracket && token.value == "[" && std::find(directives.begin(), directives.end(), toLower(filteredTokens[i + 1].value)) != directives.end())
         || std::find(directives.begin(), directives.end(), lowerVal) != directives.end())
        {
            if (token.type == Token::Type::Bracket)
                i++;
            const Token::Token& directive = filteredTokens[i];
            const std::string& lowerDir = toLower(directive.value);
            
            if (lowerDir.compare("section") == 0 || lowerDir.compare("segment") == 0)
            {
                // TODO: currently case insensitive
                std::string name = toLower(filteredTokens[++i].value);

                auto it = std::find_if(sections.begin(), sections.end(), [&](const ::Parser::Section& s) {return s.name == name; });

                if (it == sections.end())
                {
                    // Create new section
                    ::Parser::Section section;
                    section.name = name;
                    sections.emplace_back(section);
                    currentSection = &sections.back();
                }
                else
                {
                    currentSection = &(*it);
                }

                i++;

                if (toLower(filteredTokens[i].value).find("align") == 0)
                {
                    size_t pos = filteredTokens[i].value.find("=");
                    if (pos != 5)
                        context.warningManager->add(Warning::GeneralWarning("Unset section attribute 'align'", filteredTokens[i].line, filteredTokens[i].column));
                    
                    // TODO
                }
            }
            else if (lowerDir.compare("bits") == 0)
            {
                std::string bits = filteredTokens[i + 1].value;

                if (bits.compare(0, 2, "16") == 0)
                    currentBitMode = BitMode::Bits16;
                else if (bits.compare(0, 2, "32") == 0)
                    currentBitMode = BitMode::Bits32;
                else if (bits.compare(0, 2, "64") == 0)
                    currentBitMode = BitMode::Bits64;
                else
                    throw Exception::SyntaxError("Undefined bits", token.line, token.column);
            }
            else if (lowerDir.compare("org") == 0)
            {
                org = filteredTokens[i + 1].value;
            }
            else if (lowerDir.compare("align") == 0)
            {
                ::Parser::Alignment align;
                align.lineNumber = directive.line;
                align.column = directive.column;
                i++;
                // TODO: strange way
                while (i < filteredTokens.size())
                {
                    if (filteredTokens[i].type == Token::Type::Token
                    || filteredTokens[i].type == Token::Type::Operator
                    || filteredTokens[i].type == Token::Type::Character
                    || filteredTokens[i].type == Token::Type::Bracket)
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                        align.align.operands.push_back(op);
                    }
                    else
                        throw Exception::SyntaxError("Unknown value type after 'align'", filteredTokens[i].line, filteredTokens[i].column);
                    
                    i++;
                    if ( i < filteredTokens.size() && filteredTokens[i].type == Token::Type::EOL)
                        break;
                }
                i--;

                currentSection->entries.push_back(align);
            }

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                i++;
            continue;
        }

        if (token.type == Token::Type::ExternLabel)
        {
            ::Parser::Label label;
            label.name = token.value;
            label.lineNumber = token.line;
            label.column = token.column;
            label.isExtern = true;

            currentSection->entries.push_back(label);

            continue;
        }

        // Labels
        if (token.type == Token::Type::Token &&
           ((filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation && filteredTokens[i + 1].value == ":" && /*TODO: not segment:offset*/ ::x86::registers.find(token.value) == ::x86::registers.end())
         || (filteredTokens[i + 1].type == Token::Type::Token && std::find(dataDefinitions.begin(), dataDefinitions.end(), toLower(filteredTokens[i + 1].value)) != dataDefinitions.end())))
        {
            ::Parser::Label label;
            if (token.value.size() > 0 && token.value[0] == '.')
                label.name = lastMainLabel + token.value;
            else
            {
                lastMainLabel = token.value;
                label.name = token.value;
            }
            label.lineNumber = token.line;
            label.column = token.column;
            label.isExtern = false;

            if (std::find(globals.begin(), globals.end(), token.value) != globals.end())
                label.isGlobal = true;
            else
                label.isGlobal = false;

            currentSection->entries.push_back(label);

            if (filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation && filteredTokens[i + 1].value == ":")
                i++;
            continue;
        }

        // Data
        if (token.type == Token::Type::Token && std::find(dataDefinitions.begin(), dataDefinitions.end(), lowerVal) != dataDefinitions.end())
        {
            ::Parser::DataDefinition data;
            char cSize = '\0';
            if (lowerVal.compare(0, 3, "res") == 0)
            {
                data.reserved = true;
                cSize = lowerVal[3];
            }
            else if (lowerVal[0] == 'd')
            {
                data.reserved = false;
                cSize = lowerVal[1];
            }

            data.lineNumber = token.line;
            data.column = token.column;

            switch (cSize)
            {
                case 'b': data.size = 1; break;
                case 'w': data.size = 2; break;
                case 'd': data.size = 4; break;
                case 'q': data.size = 8; break;
                case 't': data.size = 10; break;
                case 'o': data.size = 16; break;
                case 'y': data.size = 32; break;
                case 'z': data.size = 64; break;
                default: throw Exception::InternalError("Unknown size suffix", token.line, token.column);
            }

            i++;
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    ::Parser::Immediate val;
                    
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                        val.operands.push_back(op);
                        i++;
                    }
                    i--;

                    data.values.push_back(val);
                }
                else if (filteredTokens[i].type == Token::Type::String)
                {
                    const std::string& val = filteredTokens[i].value;
                    size_t len = val.size();

                    for (size_t pos = 0; pos < len; pos += data.size)
                    {
                        uint64_t combined = 0;

                        for (size_t offset = 0; offset < data.size; ++offset)
                        {
                            if (pos + offset < len)
                            {
                                combined |= static_cast<uint64_t>(static_cast<unsigned char>(val[pos + offset])) << (8 * offset);
                            }
                            else
                            {
                                //add padding 0s
                                combined |= 0ULL << (8 * offset);
                            }
                        }

                        ::Parser::Immediate value;

                        ::Parser::Integer integer;
                        integer.value = combined;
                        value.operands.push_back(integer);

                        data.values.push_back(value);
                    }
                }
                else
                    throw Exception::SyntaxError("Expected definition after data definition", filteredTokens[i].line, filteredTokens[i].column);

                i++;
                if (i >= filteredTokens.size()) break;

                if (filteredTokens[i].type == Token::Type::Comma)
                {
                    i++;
                    if (i >= filteredTokens.size())
                        throw Exception::InternalError("Unexpected end after comma", filteredTokens.back().line, filteredTokens.back().column);
                }
                else if (filteredTokens[i].type != Token::Type::EOL)
                {
                    throw Exception::SyntaxError("Expected comma or end of line after data definition", filteredTokens[i].line, filteredTokens[i].column);
                }
            }

            currentSection->entries.push_back(data);

            continue;
        }

        // Instructions

        // CONTROL
        static const std::unordered_map<std::string_view, uint64_t> controlInstructions = {
            {"nop", ::x86::Instructions::NOP},
            {"hlt", ::x86::Instructions::HLT},

            {"jmp", Instructions::JMP},

            {"je", Instructions::JE}, {"jz", Instructions::JE},
            {"jne", Instructions::JNE}, {"jnz", Instructions::JNE},

            {"jg", Instructions::JG}, {"jnle", Instructions::JG},
            {"jge", Instructions::JGE}, {"jnl", Instructions::JGE},
            {"jl", Instructions::JL}, {"jnge", Instructions::JL},
            {"jle", Instructions::JLE}, {"jng", Instructions::JLE},

            {"ja", Instructions::JA}, {"jnbe", Instructions::JA},
            {"jae", Instructions::JAE}, {"jnb", Instructions::JAE},
            {"jb", Instructions::JB}, {"jnae", Instructions::JB},
            {"jbe", Instructions::JBE}, {"jna", Instructions::JBE},

            {"jo", Instructions::JO}, {"jno", Instructions::JNO},
            {"js", Instructions::JS}, {"jns", Instructions::JNS},
            {"jp", Instructions::JP}, {"jpe", Instructions::JP},
            {"jnp", Instructions::JNP}, {"jpo", Instructions::JNP},
            {"jc", Instructions::JC}, {"jnc", Instructions::JNC},

            {"ret", Instructions::RET}, {"call", Instructions::CALL}
        };

        auto it = controlInstructions.find(lowerVal);
        if (it != controlInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::NOP:
                case ::x86::Instructions::HLT:
                    break;

                // TODO: jmp short/near/far
                // TODO: call far

                case Instructions::JMP: case Instructions::CALL:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;

                        break;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'

                        break;
                    }

                    // Fallthrough
                }

                case Instructions::JE: case Instructions::JNE:
                case Instructions::JG: case Instructions::JGE:
                case Instructions::JL: case Instructions::JLE:
                case Instructions::JA: case Instructions::JAE:
                case Instructions::JB: case Instructions::JBE:
                case Instructions::JO: case Instructions::JNO:
                case Instructions::JS: case Instructions::JNS:
                case Instructions::JP: case Instructions::JNP:
                case Instructions::JC: case Instructions::JNC:
                {
                    // TODO: immediate?
                    ::Parser::Immediate imm;
                    while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                        imm.operands.push_back(op);
                        i++;
                    }
                    instruction.operands.push_back(imm);
                } break;

                case Instructions::RET:
                {
                    // TODO: immediate?
                    if (filteredTokens[i].type != Token::Type::EOL)
                    {
                        ::Parser::Immediate imm;
                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;

                default:
                    throw Exception::InternalError("Unknown control instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // INTERRUPT
        static const std::unordered_map<std::string_view, uint64_t> interruptInstructions = {
            {"int", ::x86::Instructions::INT},
            {"iret", ::x86::Instructions::IRET},
            {"iretq", ::x86::Instructions::IRETQ},
            {"iretd", ::x86::Instructions::IRETD},
            {"syscall", ::x86::Instructions::SYSCALL},
            {"sysret", x86::Instructions::SYSRET},
            {"sysenter", x86::Instructions::SYSENTER},
            {"sysexit", x86::Instructions::SYSEXIT}
        };

        it = interruptInstructions.find(lowerVal);
        if (it != interruptInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::INT:
                {
                    // TODO: immediate?
                    ::Parser::Immediate imm;
                    while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                        imm.operands.push_back(op);
                        i++;
                    }
                    instruction.operands.push_back(imm);
                } break;

                case x86::Instructions::IRET:
                case x86::Instructions::IRETQ:
                case x86::Instructions::IRETD:
                case x86::Instructions::SYSCALL:
                case x86::Instructions::SYSRET:
                case x86::Instructions::SYSENTER:
                case x86::Instructions::SYSEXIT:
                    break;

                default:
                    throw Exception::InternalError("Unknown interrupt instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // FLAGS
        static const std::unordered_map<std::string_view, uint64_t> flagInstructions = {
            {"clc", ::x86::Instructions::CLC},
            {"stc", ::x86::Instructions::STC},
            {"cmc", ::x86::Instructions::CMC},
            {"cld", ::x86::Instructions::CLD},
            {"std", ::x86::Instructions::STD},
            {"cli", ::x86::Instructions::CLI},
            {"sti", ::x86::Instructions::STI},
            {"lahf", ::x86::Instructions::LAHF},
            {"sahf", ::x86::Instructions::SAHF}
        };

        it = flagInstructions.find(lowerVal);
        if (it != flagInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::CLC:
                case ::x86::Instructions::STC:
                case ::x86::Instructions::CMC:
                case ::x86::Instructions::CLD:
                case ::x86::Instructions::STD:
                case ::x86::Instructions::CLI:
                case ::x86::Instructions::STI:
                case ::x86::Instructions::LAHF:
                case ::x86::Instructions::SAHF:
                    break;

                default:
                    throw Exception::InternalError("Unknown flag instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // STACK
        static const std::unordered_map<std::string_view, uint64_t> stackInstructions = {
            {"push", Instructions::PUSH},
            {"pop", Instructions::POP},
            {"pusha", ::x86::Instructions::PUSHA},
            {"popa", ::x86::Instructions::POPA},
            {"pushad", ::x86::Instructions::PUSHAD},
            {"popad", ::x86::Instructions::POPAD},
            {"pushf", ::x86::Instructions::PUSHF},
            {"popf", ::x86::Instructions::POPF},
            {"pushfd", ::x86::Instructions::PUSHFD},
            {"popfd", ::x86::Instructions::POPFD},
            {"pushfq", ::x86::Instructions::PUSHFQ},
            {"popfq", ::x86::Instructions::POPFQ}
        };

        it = stackInstructions.find(lowerVal);
        if (it != stackInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::PUSHA:
                case ::x86::Instructions::POPA:
                case ::x86::Instructions::PUSHAD:
                case ::x86::Instructions::POPAD:
                case ::x86::Instructions::PUSHF:
                case ::x86::Instructions::POPF:
                case ::x86::Instructions::PUSHFD:
                case ::x86::Instructions::POPFD:
                case ::x86::Instructions::PUSHFQ:
                case ::x86::Instructions::POPFQ:
                    break;

                case Instructions::PUSH: case Instructions::POP:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: immediate?
                        ::Parser::Immediate imm;

                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;
                
                default:
                    throw Exception::InternalError("Unknown stack instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);

            currentSection->entries.push_back(instruction);
            continue;
        }

        // Data
        static const std::unordered_map<std::string_view, uint64_t> dataInstructions = {
            {"mov", ::x86::Instructions::MOV}
        };

        it = dataInstructions.find(lowerVal);
        if (it != dataInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::MOV:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: Error
                    }

                    if (filteredTokens[i].type != Token::Type::Comma)
                        throw Exception::SyntaxError("Expected ',' after first argument for 'mov'", operand1.line, operand1.column);
                    i++;

                    const Token::Token& operand2 = filteredTokens[i];
                    regIt = ::x86::registers.find(operand2.value);
                    ptrsizeIt = pointer_sizes.find(operand2.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand2.type == Token::Type::Bracket && operand2.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: immediate?
                        ::Parser::Immediate imm;

                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;

                default:
                    throw Exception::InternalError("Unknown data instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // ALU
        static const std::unordered_map<std::string_view, uint64_t> aluInstructions = {
            {"add", ::x86::Instructions::ADD},
            {"adc", ::x86::Instructions::ADC},
            {"sub", ::x86::Instructions::SUB},
            {"sbb", ::x86::Instructions::SBB},
            {"cmp", ::x86::Instructions::CMP},
            {"test", ::x86::Instructions::TEST},
            {"and", ::x86::Instructions::AND},
            {"or", ::x86::Instructions::OR},
            {"xor", ::x86::Instructions::XOR},

            {"mul", ::x86::Instructions::MUL},
            {"imul", ::x86::Instructions::IMUL},
            {"div", ::x86::Instructions::DIV},
            {"idiv", ::x86::Instructions::IDIV},

            {"shl", ::x86::Instructions::SHL},
            {"shr", ::x86::Instructions::SHR},
            {"sal", ::x86::Instructions::SAL},
            {"sar", ::x86::Instructions::SAR},
            {"rol", ::x86::Instructions::ROL},
            {"ror", ::x86::Instructions::ROR},
            {"rcl", ::x86::Instructions::RCL},
            {"rcr", ::x86::Instructions::RCR},

            {"not", ::x86::Instructions::NOT},
            {"inc", ::x86::Instructions::INC},
            {"dec", ::x86::Instructions::DEC},
            {"neg", ::x86::Instructions::NEG}
        };

        it = aluInstructions.find(lowerVal);
        if (it != aluInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::ADD: case ::x86::Instructions::ADC:
                case ::x86::Instructions::SUB: case ::x86::Instructions::SBB:
                case ::x86::Instructions::CMP: case ::x86::Instructions::TEST:
                case ::x86::Instructions::AND: case ::x86::Instructions::OR:
                case ::x86::Instructions::XOR:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: Error
                    }

                    if (filteredTokens[i].type != Token::Type::Comma)
                        throw Exception::SyntaxError(std::string("Expected ',' after first argument for '") + lowerVal + "'", operand1.line, operand1.column);
                    i++;

                    const Token::Token& operand2 = filteredTokens[i];
                    regIt = ::x86::registers.find(operand2.value);
                    ptrsizeIt = pointer_sizes.find(operand2.value);

                    if (regIt != ::x86::registers.end()
                    && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand2.type == Token::Type::Bracket && operand2.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: immediate?
                        ::Parser::Immediate imm;

                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;

                case ::x86::Instructions::MUL:
                case ::x86::Instructions::DIV: case ::x86::Instructions::IDIV:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                    && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                } break;

                case ::x86::Instructions::IMUL:
                {
                    bool firstReg = false;

                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);
                    
                    if (regIt != ::x86::registers.end()
                    && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;

                        firstReg = true;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }

                    if (firstReg && filteredTokens[i].type == Token::Type::Comma)
                    {
                        i++;

                        const Token::Token& operand2 = filteredTokens[i];
                        regIt = ::x86::registers.find(operand2.value);
                        ptrsizeIt = pointer_sizes.find(operand2.value);

                        if (regIt != ::x86::registers.end()
                        && filteredTokens[i + 1].type != Token::Type::Punctuation)
                        {
                            // reg
                            ::Parser::Instruction::Register reg;
                            reg.reg = regIt->second;
                            instruction.operands.push_back(reg);
                            i++;
                        }
                        else if (ptrsizeIt != pointer_sizes.end()
                                || (operand2.type == Token::Type::Bracket && operand2.value == "[")
                                || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                        {
                            std::vector<const Token::Token*> memoryTokens;

                            if (ptrsizeIt != pointer_sizes.end())
                                i++;

                            // TODO: segment registers

                            i++; // '['

                            while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                            {
                                memoryTokens.push_back(&filteredTokens[i]);
                                i++;
                            }

                            ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                            instruction.operands.push_back(mem);

                            i++; // ']'
                        }
                        else
                        {
                            throw Exception::SyntaxError("Expected register or memory operand after ','", operand1.line, operand1.column);
                        }

                        if (filteredTokens[i].type == Token::Type::Comma)
                        {
                            i++;

                            // TODO: immediate?
                            ::Parser::Immediate imm;
                            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                            {
                                ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                                imm.operands.push_back(op);
                                i++;
                            }
                            instruction.operands.push_back(imm);
                        }
                    }
                } break;

                case ::x86::Instructions::SHL: case ::x86::Instructions::SHR:
                case ::x86::Instructions::SAL: case ::x86::Instructions::SAR:
                case ::x86::Instructions::ROL: case ::x86::Instructions::ROR:
                case ::x86::Instructions::RCL: case ::x86::Instructions::RCR:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                    else
                    {
                        // TODO: Error
                    }

                    if (filteredTokens[i].type != Token::Type::Comma)
                        throw Exception::SyntaxError(std::string("Expected ',' after first argument for '") + lowerVal + "'", operand1.line, operand1.column);
                    i++;

                    const Token::Token& operand2 = filteredTokens[i];
                    regIt = ::x86::registers.find(operand2.value);
                    ptrsizeIt = pointer_sizes.find(operand2.value);

                    if (regIt != ::x86::registers.end()
                    && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;

                        if (reg.reg != x86::Registers::CL)
                            throw Exception::SemanticError("Only CL register is allowed as shift/rotate count", operand2.line, operand2.column);

                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand2.type == Token::Type::Bracket && operand2.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        throw Exception::SyntaxError("Invalid shift/rotate count operand, must be immediate or CL", operand2.line, operand2.column);
                    }
                    else
                    {
                        // TODO: immediate?
                        ::Parser::Immediate imm;

                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i], lastMainLabel);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;

                case ::x86::Instructions::NOT: case ::x86::Instructions::INC:
                case ::x86::Instructions::DEC: case ::x86::Instructions::NEG:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    auto ptrsizeIt = pointer_sizes.find(operand1.value);

                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if (ptrsizeIt != pointer_sizes.end()
                             || (operand1.type == Token::Type::Bracket && operand1.value == "[")
                             || (regIt != ::x86::registers.end() && filteredTokens.size() > i+1 && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        std::vector<const Token::Token*> memoryTokens;

                        if (ptrsizeIt != pointer_sizes.end())
                            i++;

                        // TODO: segment registers

                        i++; // '['

                        while (!(filteredTokens[i].type == Token::Type::Bracket && filteredTokens[i].value == "]"))
                        {
                            memoryTokens.push_back(&filteredTokens[i]);
                            i++;
                        }

                        ::Parser::Instruction::Memory mem = parseMemoryOperand(memoryTokens, ptrsizeIt, pointer_sizes.end());

                        instruction.operands.push_back(mem);

                        i++; // ']'
                    }
                } break;

                default:
                    throw Exception::InternalError("Unknown data instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }


        context.warningManager->add(Warning::GeneralWarning("Unhandled token: " + token.what(&context)));
    }

    if (!implicitSection.entries.empty())
    {
        ::Parser::Section* textSection = nullptr;

        for (auto& section : sections)
        {
            if (section.name == ".text")
            {
                textSection = &section;
                break;
            }
        }

        if (textSection)
        {
            textSection->entries.insert(textSection->entries.begin(), implicitSection.entries.begin(), implicitSection.entries.end());
        }
        else
        {
            implicitSection.name = ".text";
            sections.insert(sections.begin(), implicitSection);
        }
    }
}
