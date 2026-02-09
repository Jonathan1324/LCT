#pragma once

#include <string>
#include <vector>
#include <IntTypesC.h>
#include "../Tokenizer.hpp"
#include <x86/Registers.hpp>

#include <memory>

class ExpressionParser
{
public:
    struct ExprNode {
        enum class Type
        {
            NUMBER,
            REGISTER,
            LABEL,
            BINARY_OP,
            UNARY_OP
        };

        Type type;

        uint64_t value = 0;
        std::string name;
        uint64_t reg;

        char op = '\0';
        char unary_op = '\0';

        std::shared_ptr<ExprNode> left = nullptr;
        std::shared_ptr<ExprNode> right = nullptr;
        std::shared_ptr<ExprNode> operand = nullptr;
    };

    struct AddressingMode {
        uint64_t reg1 = 0;
        uint64_t reg2 = 0;

        std::shared_ptr<ExprNode> scale1;
        std::shared_ptr<ExprNode> scale2;

        std::shared_ptr<ExprNode> displacement;

        bool has_reg1 = false;
        bool has_reg2 = false;
        bool has_displacement = false;
    };

    std::shared_ptr<ExprNode> parse(const std::vector<const Token::Token*>& tok);
    void printTree(const std::shared_ptr<ExprNode>& e, int indent = 0);

    std::shared_ptr<ExprNode> simplify(std::shared_ptr<ExprNode> e);

    AddressingMode extractAddressingMode(std::shared_ptr<ExprNode> e);

private:
    std::vector<const Token::Token*> tokens;
    uint64_t current = 0;

    const Token::Token* peek() const;
    const Token::Token* advance();

    std::shared_ptr<ExprNode> parseExpression();
    std::shared_ptr<ExprNode> parseTerm();
    std::shared_ptr<ExprNode> parseUnary();
    std::shared_ptr<ExprNode> parsePrimary();
    
    bool isNumber(const std::string& s);

    bool isRegister(const std::string& s);
    uint64_t getRegister(const std::string& s);

    bool hasRegister(const std::shared_ptr<ExprNode>& e);
};
