#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

typedef enum TokenType {
    INT_LIT,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    _EOF,
} TokenType;

typedef struct Token {
    TokenType type;
    std::string value;
} Token;

typedef enum NodeType {
    Root,
    Operator,
    Number,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(const NodeType& t, const std::string& v) : type(t), value(v) {}
} ASTNode;

std::string print_token_type(TokenType token_type)
{
    switch (token_type) {
        case INT_LIT: return "INT_LIT";
        case OPERATOR_PLUS: return "OPERATOR_PLUS";
        case OPERATOR_MINUS: return "OPERATOR_MINUS";
        case _EOF: return "EOF";
    }
}

std::string print_node_type(NodeType node_type)
{
    switch (node_type) {
        case NodeType::Root: return "Root";
        case NodeType::Operator: return "Operator";
        case NodeType::Number: return "Number";
    }
}

std::vector<Token> tokenize(const std::string contents)
{
    std::vector<Token> tokens;

    std::string buffer;
    for (int i = 0; i < contents.length(); i++) {
        char current_char = contents.at(i);

        if (std::isalpha(current_char)) {
            buffer.push_back(current_char);

            i++;

            while (std::isalnum(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
            }

            i--;

            printf("Unknown word \"%s\".\n", buffer.c_str());
            exit(EXIT_FAILURE);
        } else if (std::isdigit(current_char)) {
            buffer.push_back(current_char);
            i++;

            while (std::isdigit(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
            }

            i--;

            Token token;
            token.type = INT_LIT;
            token.value = buffer;
            tokens.push_back(token);
            buffer.clear();
        } else if (current_char == '+') {
            Token token;
            token.type = OPERATOR_PLUS;
            tokens.push_back(token);
        } else if (current_char == '-') {
            Token token;
            token.type = OPERATOR_MINUS;
            tokens.push_back(token);
        } else if (isspace(current_char)) {
            continue;
        } else {
            printf("Syntax error\n");
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}

int current = 0;

Token peek(const std::vector<Token>* tokens)
{
    if (current < tokens->size()) {
        return (*tokens)[current];
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

    printf("UNEXPECTED FACTOR");
    exit(EXIT_FAILURE);
}

std::shared_ptr<ASTNode> parse_expression(const std::vector<Token>* tokens)
{
    std::shared_ptr<ASTNode> node = parse_factor(tokens);

    while (match(tokens, TokenType::OPERATOR_PLUS) || match(tokens, TokenType::OPERATOR_MINUS)) {
        TokenType type = tokens->at(current - 1).type;
        std::shared_ptr<ASTNode> right = parse_factor(tokens);

        std::string operation;
        if (type == TokenType::OPERATOR_MINUS) {
            operation = "-";
        } else if (type == TokenType::OPERATOR_PLUS) {
            operation = "+";
        }

        std::shared_ptr<ASTNode> newNode = std::make_shared<ASTNode>(NodeType::Operator, operation);
        newNode->children.push_back(node);
        newNode->children.push_back(right);
        node = newNode;
    }

    return node;
}

std::shared_ptr<ASTNode> parse(const std::vector<Token>* tokens)
{
    current = 0;

    std::shared_ptr<ASTNode> node = parse_expression(tokens);

    return node;
}

void print_ast(const std::shared_ptr<ASTNode>& node, int depth = 0) {
    if (!node) return;
    printf("%s", std::string(std::string(depth * 2, ' ') + print_node_type(node->type) + ": " + node->value + "\n").c_str());
    for (const auto& child : node->children) {
        print_ast(child, depth + 1);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("No file path provided.\n");
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Token> tokens = tokenize(contents);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    printf("Tokenizer took %lld ms\n", milliseconds);

    printf("\n");
    for (int i = 0; i < tokens.size(); i++) {
        printf("%d: %s %s\n", i, print_token_type(tokens[i].type).c_str(), tokens[i].value.c_str());
    }

    std::shared_ptr<ASTNode> root_node = parse(&tokens);
    printf("\n");
    print_ast(root_node);

    return 0;
}
