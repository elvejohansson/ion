#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

enum TokenType {
    INT_LIT,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_STAR,
    OPERATOR_SLASH,
    _EOF,
};

typedef struct Token {
    TokenType type;
    std::string value;
} Token;

typedef enum NodeType {
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
        case TokenType::INT_LIT: return "INT_LIT";
        case TokenType::OPERATOR_PLUS: return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MINUS: return "OPERATOR_MINUS";
        case TokenType::OPERATOR_STAR: return "OPERATOR_STAR";
        case TokenType::OPERATOR_SLASH: return "OPERATOR_SLASH";
        case TokenType::_EOF: return "EOF";
    }
}

std::string print_node_type(NodeType node_type)
{
    switch (node_type) {
        case NodeType::Operator: return "Operator";
        case NodeType::Number: return "Number";
    }
}

std::vector<Token> tokenize(const std::string contents)
{
    std::vector<Token> tokens;

    int line_count = 1;
    int char_count = 1;
    std::string buffer;

    for (int i = 0; i < contents.length(); i++) {
        char current_char = contents.at(i);

        if (std::isalpha(current_char)) {
            buffer.push_back(current_char);

            i++;
            char_count++;

            while (std::isalnum(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
                char_count++;
            }

            i--;

            printf("Unknown keyword on line %d: \"%s\".\n", line_count, buffer.c_str());
            exit(EXIT_FAILURE);
        } else if (std::isdigit(current_char)) {
            buffer.push_back(current_char);
            i++;
            char_count++;

            while (std::isdigit(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
                char_count++;
            }

            i--;

            Token token;
            token.type = TokenType::INT_LIT;
            token.value = buffer;
            tokens.push_back(token);
            buffer.clear();
        } else if (current_char == '+') {
            Token token;
            token.type = TokenType::OPERATOR_PLUS;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == '-') {
            Token token;
            token.type = TokenType::OPERATOR_MINUS;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == '*') {
            Token token;
            token.type = TokenType::OPERATOR_STAR;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == '/') {
            Token token;
            token.type = TokenType::OPERATOR_SLASH;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == '\n') {
            line_count++;
            char_count = 1;
            continue;
        } else if (isspace(current_char)) {
            char_count++;
            continue;
        } else {
            printf("Syntax error at %d:%d\n.", line_count, char_count);
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

        std::shared_ptr<ASTNode> newNode = std::make_shared<ASTNode>(NodeType::Operator, operation);
        newNode->children.push_back(node);
        newNode->children.push_back(right);
        node = newNode;
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

void generate_code(const std::shared_ptr<ASTNode> node, std::stringstream& stream)
{
    if (node->type == NodeType::Number) {
        stream << "mov x0, #" << node->value << "\n";
    } else if (node->type == NodeType::Operator) {
        generate_code(node->children[0], stream);
        stream << "str x0, [sp, -16]!\n";

        generate_code(node->children[1], stream);
        stream << "ldr x1, [sp], 16\n";

        if (node->value == "+") {
            stream << "add x0, x1, x0\n";
        } else if (node->value == "-") {
            stream << "sub x0, x1, x0\n";
        } else if (node->value == "*") {
            stream << "mul x0, x1, x0\n";
        } else if (node->value == "/") {
            stream << "sdiv x0, x1, x0\n";
        }
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

    auto tokenizer_start = std::chrono::high_resolution_clock::now();
    std::vector<Token> tokens = tokenize(contents);
    auto tokenizer_elapsed = std::chrono::high_resolution_clock::now() - tokenizer_start;

    printf("\n");
    for (int i = 0; i < tokens.size(); i++) {
        printf("%d: %s %s\n", i, print_token_type(tokens[i].type).c_str(), tokens[i].value.c_str());
    }

    auto parser_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<ASTNode> node = parse(&tokens);
    auto parser_elapsed = std::chrono::high_resolution_clock::now() - parser_start;

    printf("\n");
    print_ast(node);

    auto generator_start = std::chrono::high_resolution_clock::now();
    std::stringstream buffer;
    buffer << ".global _start\n";
    buffer << ".text\n";
    buffer << "_start:\n";

    generate_code(node, buffer);

    buffer << "mov x16, #1\n";
    buffer << "svc #0\n";

    std::ofstream program;
    program.open("program.s");
    program << buffer.str();
    program.close();

    auto generator_elapsed = std::chrono::high_resolution_clock::now() - parser_start;

    long long tokenizer_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(tokenizer_elapsed).count();
    long long parser_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(parser_elapsed).count();
    long long generator_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(generator_elapsed).count();

    printf("\n");
    printf("Tokenizer took %lld μs\n", tokenizer_microseconds);
    printf("Parser took %lld μs\n", parser_microseconds);
    printf("Code generation took %lld μs\n", generator_microseconds);

    return 0;
}
