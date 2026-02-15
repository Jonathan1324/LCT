#pragma once

#include "../Parser.hpp"
#include <x86/Registers.hpp>

namespace x86
{
    class Parser : public ::Parser::Parser
    {
    public:
        Parser(const Context& _context, Architecture _arch, BitMode _bits);
        ~Parser() = default;
        
        void Parse(const std::vector<Token::Token>& tokens) override;
    
    protected:
        ::Parser::Instruction::Register getReg(const Token::Token& token);

        ::Parser::Instruction::Memory parseMemoryOperand(
            const std::vector<const Token::Token*> tokens,
            std::unordered_map<std::string_view, uint64_t>::const_iterator ptrsizeIt,
            std::unordered_map<std::string_view, uint64_t>::const_iterator ptrsizeEnd
        );

        static const std::unordered_map<std::string_view, uint64_t> pointer_sizes;
    };

    inline const std::unordered_map<std::string_view, uint64_t> Parser::pointer_sizes = {
        {"byte", 8}, {"word", 16}, {"dword", 32}, {"qword", 64}
    };

    inline ::Parser::Instruction::Register Parser::getReg(const Token::Token& token)
    {
        ::Parser::Instruction::Register reg;
        auto it = ::x86::registers.find(token.value.c_str());
        if (it == ::x86::registers.end()) throw Exception::InternalError(std::string("Unknown register: ") + token.value.c_str(), token.line, token.column);
        reg.reg = it->second;
        return reg;
    }
}