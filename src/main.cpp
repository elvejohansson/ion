#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#if defined (__APPLE__)
    #define _MAC_OS
#endif

#if defined (__aarch64__)
    #define _ARM64
#endif

enum TokenType {
    INT_LIT,        // 123
    IDENTIFIER,     // xy
    ASSIGNMENT,     // =

    OPERATOR_PLUS,  // +
    OPERATOR_MINUS, // -
    OPERATOR_STAR,  // *
    OPERATOR_SLASH, // /

    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )

    _EOF,
};

struct Token {
    TokenType type;
    std::string value;
};

enum NodeType {
    Root,
    Assignment,
    BinaryOperator,
    Number,
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(const NodeType& t, const std::string& v) : type(t), value(v) {}
};

const char* print_token_type(TokenType token_type)
{
    switch (token_type) {
        case TokenType::INT_LIT: return "INT_LIT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::ASSIGNMENT: return "ASSIGNMENT";
        case TokenType::OPERATOR_PLUS: return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MINUS: return "OPERATOR_MINUS";
        case TokenType::OPERATOR_STAR: return "OPERATOR_STAR";
        case TokenType::OPERATOR_SLASH: return "OPERATOR_SLASH";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::_EOF: return "EOF";
    }
}

const char* print_node_type(NodeType node_type)
{
    switch (node_type) {
        case NodeType::Root: return "Root";
        case NodeType::Assignment: return "Assignment";
        case NodeType::BinaryOperator: return "Operator";
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

            Token token;
            token.type = TokenType::IDENTIFIER;
            token.value = buffer;
            tokens.push_back(token);
            buffer.clear();
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
        } else if (current_char == '(') {
            Token token;
            token.type = TokenType::LEFT_PAREN;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == ')') {
            Token token;
            token.type = TokenType::RIGHT_PAREN;
            tokens.push_back(token);

            char_count++;
        } else if (current_char == '=') {
            Token token;
            token.type = TokenType::ASSIGNMENT;
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

    // lets push this on at the end to make life easier in the future
    // (i have no idea if this will actually bring any benefits)
    Token token;
    token.type = TokenType::_EOF;
    tokens.push_back(token);

    return tokens;
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

std::shared_ptr<ASTNode> parse_factor(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_term(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_expression(const std::vector<Token>* tokens);
std::shared_ptr<ASTNode> parse_statement(const std::vector<Token>* tokens);

std::shared_ptr<ASTNode> parse_factor(const std::vector<Token>* tokens)
{
    if (match(tokens, TokenType::INT_LIT)) {
        return std::make_shared<ASTNode>(NodeType::Number, tokens->at(current - 1).value);
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

    printf("Syntax error, unexpected type %s.", print_token_type(peek(tokens).type));
    exit(EXIT_FAILURE);

    // return parse_expression(tokens);
}

std::shared_ptr<ASTNode> parse(const std::vector<Token>* tokens, const std::shared_ptr<ASTNode> root_node)
{
    current = 0;

    while (peek(tokens).type != TokenType::_EOF) {
        auto statement_node = parse_statement(tokens);

        root_node->children.push_back(statement_node);
    }

    return root_node;
}

void print_ast(const std::shared_ptr<ASTNode>& node, int depth = 0) {
    auto indent_string = std::string(depth * 2, ' ');

    printf("%s%s: %s\n", indent_string.c_str(), print_node_type(node->type), node->value.c_str());
    for (const auto& child : node->children) {
        print_ast(child, depth + 1);
    }
}

std::unordered_map<std::string, int> symbol_table;
int current_offset = -16;

int get_variable_offset(const std::string& variable_name) {

    if (symbol_table.find(variable_name) == symbol_table.end()) {
        symbol_table[variable_name] = current_offset;
        current_offset -= 16;
    }

    return symbol_table[variable_name];
}

void generate_code(const std::shared_ptr<ASTNode> node, std::stringstream& stream)
{
    switch (node->type) {
        case NodeType::Root: {
            for (int i = 0; i < node->children.size(); i++) {
                generate_code(node->children[i], stream);
            }
            break;
        }
        case NodeType::Number: {
            stream << "\tmov x0, #" << node->value << "\n";
            break;
        }
        case NodeType::BinaryOperator: {
            generate_code(node->children[0], stream);
            stream << "\tstr x0, [sp, -16]!\n";

            generate_code(node->children[1], stream);
            stream << "\tldr x1, [sp], 16\n";

            if (node->value == "+") {
                stream << "\tadd x0, x1, x0\n";
            } else if (node->value == "-") {
                stream << "\tsub x0, x1, x0\n";
            } else if (node->value == "*") {
                stream << "\tmul x0, x1, x0\n";
            } else if (node->value == "/") {
                stream << "\tsdiv x0, x1, x0\n";
            }

            break;
        }
        case NodeType::Assignment: {
            generate_code(node->children[0], stream);

            std::string variable_name = node->value;
            int stack_offset = get_variable_offset(variable_name);

            stream << "\tstr x0, [sp, " << stack_offset << "]\n";

            break;
        }
    }
}

void compile_program(const std::stringstream& stream)
{
    std::ofstream program;
    program.open("./build/program.s");
    program << stream.str();
    program.close();

#if defined (_MAC_OS) && defined (_ARM64)
    std::system("as ./build/program.s -o ./build/program.o");
    std::system("ld ./build/program.o -o ./build/program -e _start");
    printf("\nSuccessfully compiled program.\n");
#else
    printf("\nUnsupported compilation architecture, exiting...\n");
    exit(EXIT_FAILURE);
#endif
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
        printf("%d: %s %s\n", i, print_token_type(tokens[i].type), tokens[i].value.c_str());
    }

    auto parser_start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<ASTNode> root_node = std::make_shared<ASTNode>(NodeType::Root, "");
    parse(&tokens, root_node);
    auto parser_elapsed = std::chrono::high_resolution_clock::now() - parser_start;

    printf("\n");
    print_ast(root_node);

    auto generator_start = std::chrono::high_resolution_clock::now();
    std::stringstream buffer;
    buffer << ".global _start\n";
    buffer << ".text\n";
    buffer << "\n_start:\n";

    generate_code(root_node, buffer);

    buffer << "\tmov x16, #1\n";
    buffer << "\tsvc #0\n";

    auto generator_elapsed = std::chrono::high_resolution_clock::now() - generator_start;

    auto compilation_start = std::chrono::high_resolution_clock::now();
    compile_program(buffer);
    auto compilation_elapsed = std::chrono::high_resolution_clock::now() - compilation_start;

    float tokenizer_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(tokenizer_elapsed).count();
    float parser_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(parser_elapsed).count();
    float generator_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(generator_elapsed).count();
    long long compilation_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(compilation_elapsed).count();

    printf("\n");
    printf("Tokenization took %f ms\n", tokenizer_microseconds / 1000);
    printf("Parsing took %f ms\n", parser_microseconds / 1000);
    printf("Code generation took %f ms\n", generator_microseconds / 1000);
    printf("Compilation took %lld ms\n", compilation_milliseconds);

    return 0;
}
