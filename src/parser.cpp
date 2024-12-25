#include "parser.hpp"
#include "lexer.hpp"

const char* print_node_type(NodeType node_type)
{
    switch (node_type) {
        case NodeType::Root: return "Root";
        case NodeType::Assignment: return "Assignment";
        case NodeType::BinaryOperator: return "Operator";
        case NodeType::ConditionOperator: return "ConditionOperator";
        case NodeType::Number: return "Number";
        case NodeType::Identifier: return "Identifier";
        case NodeType::Boolean: return "Boolean";
        case NodeType::If: return "If";
        case NodeType::Else: return "Else";
        case NodeType::Block: return "Block";
    }
}

int current = 0;

Token peek(const std::vector<Token>* tokens, int lookahead = 0)
{
    if (current + lookahead < tokens->size()) {
        return (*tokens)[current + lookahead];
    }

    return { TokenType::_EOF };
}

Token advance(const std::vector<Token>* tokens)
{
    if (current < tokens->size()) {
        return (*tokens)[current++];
    }

    return { TokenType::_EOF };
}

const bool match(const std::vector<Token>* tokens, TokenType type)
{
    if (peek(tokens).type == type) {
        advance(tokens);
        return true;
    }

    return false;
}

std::shared_ptr<ASTNode> parse_factor(const std::vector<Token>* tokens)
{
    if (match(tokens, TokenType::INT_LIT)) {
        return std::make_shared<ASTNode>(NodeType::Number, tokens->at(current - 1).value);
    }

    if (match(tokens, TokenType::IDENTIFIER)) {
        return std::make_shared<ASTNode>(NodeType::Identifier, tokens->at(current - 1).value);
    }

    if (match(tokens, TokenType::BOOLEAN)) {
        return std::make_shared<ASTNode>(NodeType::Boolean, tokens->at(current - 1).value);
    }

    if (match(tokens, TokenType::LEFT_PAREN)) {
        std::shared_ptr<ASTNode> inner = parse_expression(tokens);

        if (!match(tokens, TokenType::RIGHT_PAREN)) {
            printf("No matching closing parentheses found.\n");
            exit(EXIT_FAILURE);
        }

        return inner;
    }

    printf("UNEXPECTED FACTOR");
    exit(EXIT_FAILURE);
}

std::shared_ptr<ASTNode> parse_term(const std::vector<Token>* tokens)
{
    std::shared_ptr<ASTNode> node = parse_factor(tokens);

    while (match(tokens, TokenType::OPERATOR_STAR) || match(tokens, TokenType::OPERATOR_SLASH)) {
        TokenType type = tokens->at(current - 1).type;
        std::shared_ptr<ASTNode> right = parse_factor(tokens);

        std::string operation;
        if (type == TokenType::OPERATOR_STAR) {
            operation = "*";
        } else if (type == TokenType::OPERATOR_SLASH) {
            operation = "/";
        }

        std::shared_ptr<ASTNode> operator_node = std::make_shared<ASTNode>(NodeType::BinaryOperator, operation);
        operator_node->children.push_back(node);
        operator_node->children.push_back(right);
        node = operator_node;
    }

    return node;
}

std::shared_ptr<ASTNode> parse_expression(const std::vector<Token>* tokens)
{
    std::shared_ptr<ASTNode> node = parse_term(tokens);

    while (match(tokens, TokenType::OPERATOR_PLUS) || match(tokens, TokenType::OPERATOR_MINUS)) {
        TokenType type = tokens->at(current - 1).type;
        std::shared_ptr<ASTNode> right = parse_term(tokens);

        std::string operation;
        if (type == TokenType::OPERATOR_MINUS) {
            operation = "-";
        } else if (type == TokenType::OPERATOR_PLUS) {
            operation = "+";
        }

        std::shared_ptr<ASTNode> newNode = std::make_shared<ASTNode>(NodeType::BinaryOperator, operation);
        newNode->children.push_back(node);
        newNode->children.push_back(right);
        node = newNode;
    }

    while (match(tokens, TokenType::CONDITION_OPERATOR_EQ)) {
        TokenType type = tokens->at(current - 1).type;
        std::shared_ptr<ASTNode> right = parse_term(tokens);

        std::string operation;
        if (type == TokenType::CONDITION_OPERATOR_EQ) {
            operation = "==";
        }

        std::shared_ptr<ASTNode> condition_node = std::make_shared<ASTNode>(NodeType::ConditionOperator, operation);
        condition_node->children.push_back(node);
        condition_node->children.push_back(right);
        node = condition_node;
    }

    return node;
}

std::shared_ptr<ASTNode> parse_statement(const std::vector<Token>* tokens)
{
    if (peek(tokens).type == TokenType::IDENTIFIER && peek(tokens, 1).type == TokenType::ASSIGNMENT) {
        Token identifier_node = advance(tokens);
        advance(tokens); // discard assignment operator

        std::shared_ptr<ASTNode> assignment_expression_node = parse_expression(tokens);

        std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>(
                NodeType::Assignment,
                identifier_node.value
        );
        node->children.push_back(assignment_expression_node);

        return node;
    }

    if (peek(tokens).type == TokenType::IF && peek(tokens, 1).type == TokenType::LEFT_PAREN) {
        std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>(NodeType::If, "");
        advance(tokens);

        auto if_expression = parse_expression(tokens);

        node->children.push_back(if_expression);

        if (peek(tokens).type != TokenType::LEFT_BRACE) {
            printf("Syntax error, expected opening brace after conditional expression.\n");
            exit(EXIT_FAILURE);
        }

        advance(tokens); // skip the first brace

        std::shared_ptr<ASTNode> block_node = std::make_shared<ASTNode>(NodeType::Block, "");

        while (!match(tokens, TokenType::RIGHT_BRACE)) {
            std::shared_ptr<ASTNode> child_statement = parse_statement(tokens);

            block_node->children.push_back(child_statement);
        }

        node->children.push_back(block_node);

        if (peek(tokens).type == TokenType::ELSE && peek(tokens, 1).type == TokenType::LEFT_BRACE) {
            std::shared_ptr<ASTNode> else_node = std::make_shared<ASTNode>(NodeType::Else, "");
            std::shared_ptr<ASTNode> else_block_node = std::make_shared<ASTNode>(NodeType::Block, "");

            else_node->children.push_back(else_block_node);

            advance(tokens);
            advance(tokens);

            while (!match(tokens, TokenType::RIGHT_BRACE)) {
                std::shared_ptr<ASTNode> else_child_statement = parse_statement(tokens);

                else_block_node->children.push_back(else_child_statement);
            }

            node->children.push_back(else_node);
        }

        return node;
    }

    auto token = peek(tokens);

    printf("\n\x1b[31m[error 1]\033[0m: %s was not expected here.\n", print_token_type(token.type));
    printf("\t-> test.ion:%d:%d\n", token.line, token.character);
    printf("\n");

    exit(EXIT_FAILURE);

    // return parse_expression(tokens);
}

const std::shared_ptr<ASTNode> parse(const std::vector<Token>* tokens)
{
    current = 0;

    std::shared_ptr<ASTNode> root_node = std::make_shared<ASTNode>(NodeType::Root, "");

    while (peek(tokens).type != TokenType::_EOF) {
        auto statement_node = parse_statement(tokens);

        root_node->children.push_back(statement_node);
    }

    return root_node;
}

void print_ast(const std::shared_ptr<ASTNode>& node, int depth)
{
    auto indent_string = std::string(depth * 2, ' ');

    printf("%s%s: %s\n", indent_string.c_str(), print_node_type(node->type), node->value.c_str());
    for (const auto& child : node->children) {
        print_ast(child, depth + 1);
    }
}
