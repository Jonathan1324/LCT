#include "Parser.hpp"

::Parser::Instruction::Memory x86::Parser::parseMemoryOperand(
    const std::vector<const Token::Token*> tokens,
    std::unordered_map<std::string_view, uint64_t>::const_iterator ptrsizeIt,
    std::unordered_map<std::string_view, uint64_t>::const_iterator ptrsizeEnd
)
{
    ::Parser::Instruction::Memory mem;

    if (ptrsizeIt != ptrsizeEnd)
        mem.pointer_size = ptrsizeIt->second;
    else
        mem.pointer_size = ::Parser::Instruction::Memory::NO_POINTER_SIZE;

    

    // TODO

    return mem;
}
