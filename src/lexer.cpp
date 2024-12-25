#include <vector>

#include "lexer.hpp"

const char* print_token_type(TokenType token_type)
{
    switch (token_type) {
        case TokenType::INT_LIT: return "INT_LIT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::ASSIGNMENT: return "ASSIGNMENT";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::OPERATOR_PLUS: return "OPERATOR_PLUS";
        case TokenType::OPERATOR_MINUS: return "OPERATOR_MINUS";
        case TokenType::OPERATOR_STAR: return "OPERATOR_STAR";
        case TokenType::OPERATOR_SLASH: return "OPERATOR_SLASH";
        case TokenType::CONDITION_OPERATOR_EQ: return "EQ";
        case TokenType::CONDITION_OPERATOR_NE: return "NE";
        case TokenType::CONDITION_OPERATOR_GT: return "GT";
        case TokenType::CONDITION_OPERATOR_LT: return "LT";
        case TokenType::CONDITION_OPERATOR_GTE: return "GTE";
        case TokenType::CONDITION_OPERATOR_LTE: return "LTE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::_EOF: return "EOF";
    }
}

std::vector<Token> tokenize(const std::string contents)
{
    std::vector<Token> tokens;

    int line_count = 1;
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

            if (buffer == "true" || buffer == "false") {
                Token token;
                token.type = TokenType::BOOLEAN;
                token.value = buffer;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "if") {
                Token token;
                token.type = TokenType::IF;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "else") {
                Token token;
                token.type = TokenType::ELSE;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            Token token;
            token.type = TokenType::IDENTIFIER;
            token.value = buffer;
            tokens.push_back(token);
            buffer.clear();
        } else if (std::isdigit(current_char)) {
            buffer.push_back(current_char);
            i++;

            while (std::isdigit(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
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
        } else if (current_char == '-') {
            Token token;
            token.type = TokenType::OPERATOR_MINUS;
            tokens.push_back(token);
        } else if (current_char == '*') {
            Token token;
            token.type = TokenType::OPERATOR_STAR;
            tokens.push_back(token);
        } else if (current_char == '/') {
            Token token;
            token.type = TokenType::OPERATOR_SLASH;
            tokens.push_back(token);
        } else if (current_char == '(') {
            Token token;
            token.type = TokenType::LEFT_PAREN;
            tokens.push_back(token);
        } else if (current_char == ')') {
            Token token;
            token.type = TokenType::RIGHT_PAREN;
            tokens.push_back(token);
        } else if (current_char == '{') {
            Token token;
            token.type = TokenType::LEFT_BRACE;
            tokens.push_back(token);
        } else if (current_char == '}') {
            Token token;
            token.type = TokenType::RIGHT_BRACE;
            tokens.push_back(token);
        } else if (current_char == '=') {
            i++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_EQ;
                tokens.push_back(token);
            } else if (std::isspace(contents.at(i))) {
                Token token;
                token.type = TokenType::ASSIGNMENT;
                tokens.push_back(token);
            } else {
                i--;
            }
        } else if (current_char == '\n') {
            line_count++;
            continue;
        } else if (isspace(current_char)) {
            continue;
        } else {
            printf("Syntax error on line %d\n.", line_count);
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
