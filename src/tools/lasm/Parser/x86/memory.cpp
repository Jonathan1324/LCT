#include "Parser.hpp"

#include "expression.hpp"

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

    for (const Token::Token* token : tokens)
    {
        if (token->type == Token::Type::Token || token->type == Token::Type::Operator)
            continue;

        if (token->type == Token::Type::Bracket)
        {
            if (token->value == "(" || token->value == ")") continue;
        }

        throw Exception::SyntaxError(
            "Invalid token in memory operand: " + token->value,
            token->line, token->column
        );
    }

    ExpressionParser exprParser;
    std::shared_ptr<ExpressionParser::ExprNode> ast = exprParser.parse(tokens);

    if (!ast)
        throw Exception::SyntaxError("Empty memory operand", -1, -1);

    exprParser.printTree(ast);

    std::cout << "Simplifying..." << std::endl;
    ast = exprParser.simplify(ast);

    exprParser.printTree(ast);

    ExpressionParser::AddressingMode mode = exprParser.extractAddressingMode(ast);

    if (mode.has_reg1)
    {
        std::cout << "Reg1:" << std::endl;
        std::cout << "  Reg: " << mode.reg1 << std::endl;
        std::cout << "  Scale:" << std::endl;
        exprParser.printTree(mode.scale1, 4);
    }
    if (mode.has_reg2)
    {
        std::cout << "Reg2:" << std::endl;
        std::cout << "  Reg: " << mode.reg2 << std::endl;
        std::cout << "  Scale:" << std::endl;
        exprParser.printTree(mode.scale2, 4);
    }

    if (mode.has_displacement)
    {
        std::cout << "Displacement:" << std::endl;
        exprParser.printTree(mode.displacement, 2);
    }

    if (mode.has_reg1)
    {
        mem.use_reg1 = true;
        mem.reg1 = mode.reg1;
        mem.scale1 = exprParser.convertToImmediate(mode.scale1);
    }

    if (mode.has_reg2)
    {
        mem.use_reg2 = true;
        mem.reg2 = mode.reg2;
        mem.scale2 = exprParser.convertToImmediate(mode.scale2);
    }

    if (mode.has_displacement)
    {
        mem.use_displacement = true;
        mem.displacement = exprParser.convertToImmediate(mode.displacement);
    }

    return mem;
}
