#include <vector>

#include "lexer.hpp"

const char* print_token_type(TokenType token_type)
{
    switch (token_type) {
        case TokenType::INT_LIT:
            return "'number'";
        case TokenType::IDENTIFIER:
            return "'identifier'";
        case TokenType::ASSIGNMENT:
            return "'='";
        case TokenType::BOOLEAN:
            return "'boolean'";
        case TokenType::IF:
            return "'if'";
        case TokenType::ELSE:
            return "'else'";
        case TokenType::OPERATOR_PLUS:
            return "'+'";
        case TokenType::OPERATOR_MINUS:
            return "'-'";
        case TokenType::OPERATOR_STAR:
            return "'*'";
        case TokenType::OPERATOR_SLASH:
            return "'/'";
        case TokenType::CONDITION_OPERATOR_EQ:
            return "'=='";
        case TokenType::LEFT_PAREN:
            return "'('";
        case TokenType::RIGHT_PAREN:
            return "')'";
        case TokenType::LEFT_BRACE:
            return "'{'";
        case TokenType::RIGHT_BRACE:
            return "'}'";
        case TokenType::_EOF:
            return "'EOF'";
    }
}

std::vector<Token> tokenize(const std::string contents)
{
    std::vector<Token> tokens;

    int line_count = 1;
    int characher_count = 1;
    std::string buffer;

    for (int i = 0; i < contents.length(); i++) {
        char current_char = contents.at(i);

        if (std::isalpha(current_char)) {
            buffer.push_back(current_char);

            i++;
            characher_count++;

            while (std::isalnum(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
                characher_count++;
            }

            i--;
            characher_count--;

            if (buffer == "true" || buffer == "false") {
                Token token;
                token.type = TokenType::BOOLEAN;
                token.value = buffer;
                token.line = line_count;
                token.character = characher_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "if") {
                Token token;
                token.type = TokenType::IF;
                token.line = line_count;
                token.character = characher_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "else") {
                Token token;
                token.type = TokenType::ELSE;
                token.line = line_count;
                token.character = characher_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            Token token;
            token.type = TokenType::IDENTIFIER;
            token.value = buffer;
            token.line = line_count;
            token.character = characher_count - buffer.length() + 1;
            tokens.push_back(token);
            buffer.clear();
        } else if (std::isdigit(current_char)) {
            buffer.push_back(current_char);
            i++;
            characher_count++;

            while (std::isdigit(contents.at(i))) {
                buffer.push_back(contents.at(i));
                i++;
                characher_count++;
            }

            i--;
            characher_count--;

            Token token;
            token.type = TokenType::INT_LIT;
            token.value = buffer;
            token.line = line_count;
            token.character = characher_count - buffer.length() + 1;
            tokens.push_back(token);
            buffer.clear();
        } else if (current_char == '+') {
            Token token;
            token.type = TokenType::OPERATOR_PLUS;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '-') {
            Token token;
            token.type = TokenType::OPERATOR_MINUS;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '*') {
            Token token;
            token.type = TokenType::OPERATOR_STAR;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '/') {
            i++;
            characher_count++;

            if (contents.at(i) == '/') {
                while (contents.at(i) != '\n') {
                    i++;
                    characher_count++;
                }
                continue;
            } else {
                i--;
                characher_count--;

                Token token;
                token.type = TokenType::OPERATOR_SLASH;
                token.line = line_count;
                token.character = characher_count;
                tokens.push_back(token);
            }
        } else if (current_char == '(') {
            Token token;
            token.type = TokenType::LEFT_PAREN;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == ')') {
            Token token;
            token.type = TokenType::RIGHT_PAREN;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '{') {
            Token token;
            token.type = TokenType::LEFT_BRACE;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '}') {
            Token token;
            token.type = TokenType::RIGHT_BRACE;
            token.line = line_count;
            token.character = characher_count;
            tokens.push_back(token);
        } else if (current_char == '=') {
            i++;
            characher_count++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_EQ;
                token.line = line_count;
                token.character = characher_count;
                tokens.push_back(token);
            } else if (std::isspace(contents.at(i))) {
                Token token;
                token.type = TokenType::ASSIGNMENT;
                token.line = line_count;
                token.character = characher_count;
                tokens.push_back(token);
            } else {
                i--;
                characher_count++;
            }
        } else if (current_char == '\n') {
            line_count++;
            characher_count = 1;
            continue;
        } else if (isspace(current_char)) {
            characher_count++;
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
    token.line = line_count;
    token.character = characher_count;
    tokens.push_back(token);

    return tokens;
}
