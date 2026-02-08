#include "Parser.hpp"

::Parser::Instruction::Memory x86::Parser::parseMemoryOperand(const std::vector<const Token::Token*> tokens)
{
    // TODO
    
    ::Parser::Instruction::Memory mem;

    mem.pointer_size = 64;

    return mem;
}
