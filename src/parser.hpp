#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

#include "lexer.hpp"

enum NodeType {
    Root,
    Assignment,
    BinaryOperator,
    ConditionOperator,
    Number,
    Identifier,
    Boolean,
    If,
    Else,
    Block,
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(const NodeType& t, const std::string& v) : type(t), value(v) {}
};

Token peek(const std::vector<Token>* tokens, int lookahead);
Token advance(const std::vector<Token>* tokens);
const bool match(const std::vector<Token>* tokens, TokenType type);

std::shared_ptr<ASTNode> parse_factor(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_term(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_expression(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_statement(const std::vector<Token>* tokens);

const std::shared_ptr<ASTNode> parse(const std::vector<Token>* tokens);

void print_ast(const std::shared_ptr<ASTNode>& node, int depth);

#endif
