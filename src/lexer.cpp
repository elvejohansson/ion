#include <cstdio>
#include <cstdlib>
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
        case TokenType::BANG:
            return "'!'";
        case TokenType::BOOLEAN:
            return "'boolean'";
        case TokenType::STRING:
            return "'string'";
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
        case TokenType::CONDITION_OPERATOR_NE:
            return "'!='";
        case TokenType::CONDITION_OPERATOR_GT:
            return "'>'";
        case TokenType::CONDITION_OPERATOR_LT:
            return "'<'";
        case TokenType::CONDITION_OPERATOR_GTE:
            return "'>='";
        case TokenType::CONDITION_OPERATOR_LTE:
            return "'<='";
        case TokenType::LEFT_PAREN:
            return "'('";
        case TokenType::RIGHT_PAREN:
            return "')'";
        case TokenType::LEFT_BRACE:
            return "'{'";
        case TokenType::RIGHT_BRACE:
            return "'}'";
        case TokenType::ASM:
            return "'#asm'";
        case TokenType::_EOF:
            return "'EOF'";
    }
}

std::vector<Token> tokenize(const std::string& contents)
{
    std::vector<Token> tokens;

    int line_count = 1;
    int character_count = 1;
    std::string buffer;

    for (int i = 0; i < contents.length(); i++) {
        char current_char = contents.at(i);

        if (std::isalpha(current_char) != 0) {
            buffer.push_back(current_char);

            i++;
            character_count++;

            while (std::isalnum(contents.at(i)) != 0) {
                buffer.push_back(contents.at(i));
                i++;
                character_count++;
            }

            i--;
            character_count--;

            if (buffer == "true" || buffer == "false") {
                Token token;
                token.type = TokenType::BOOLEAN;
                token.value = buffer;
                token.line = line_count;
                token.character = character_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "if") {
                Token token;
                token.type = TokenType::IF;
                token.line = line_count;
                token.character = character_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            if (buffer == "else") {
                Token token;
                token.type = TokenType::ELSE;
                token.line = line_count;
                token.character = character_count - buffer.length() + 1;
                tokens.push_back(token);
                buffer.clear();
                continue;
            }

            Token token;
            token.type = TokenType::IDENTIFIER;
            token.value = buffer;
            token.line = line_count;
            token.character = character_count - buffer.length() + 1;
            tokens.push_back(token);
            buffer.clear();
        } else if (std::isdigit(current_char) != 0) {
            buffer.push_back(current_char);
            i++;
            character_count++;

            while (std::isdigit(contents.at(i)) != 0) {
                buffer.push_back(contents.at(i));
                i++;
                character_count++;
            }

            i--;
            character_count--;

            Token token;
            token.type = TokenType::INT_LIT;
            token.value = buffer;
            token.line = line_count;
            token.character = character_count - buffer.length() + 1;
            tokens.push_back(token);
            buffer.clear();
        } else if (current_char == '+') {
            Token token;
            token.type = TokenType::OPERATOR_PLUS;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '-') {
            Token token;
            token.type = TokenType::OPERATOR_MINUS;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '*') {
            Token token;
            token.type = TokenType::OPERATOR_STAR;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '/') {
            i++;
            character_count++;

            if (contents.at(i) == '/') {
                while (contents.at(i) != '\n') {
                    i++;
                    character_count++;
                }
                continue;
            }

            i--;
            character_count--;

            Token token;
            token.type = TokenType::OPERATOR_SLASH;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '(') {
            Token token;
            token.type = TokenType::LEFT_PAREN;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == ')') {
            Token token;
            token.type = TokenType::RIGHT_PAREN;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '{') {
            Token token;
            token.type = TokenType::LEFT_BRACE;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '}') {
            Token token;
            token.type = TokenType::RIGHT_BRACE;
            token.line = line_count;
            token.character = character_count;
            tokens.push_back(token);
        } else if (current_char == '=') {
            i++;
            character_count++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_EQ;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else if (std::isspace(contents.at(i)) != 0) {
                Token token;
                token.type = TokenType::ASSIGNMENT;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else {
                i--;
                character_count++;
            }
        } else if (current_char == '!') {
            i++;
            character_count++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_NE;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else if (std::isalnum(contents.at(i)) != 0) {
                Token token;
                token.type = TokenType::BANG;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else {
                // @todo check me for correctness
                printf("Invalid character following '!'.\n");
                exit(EXIT_FAILURE);
            }
        } else if (current_char == '>') {
            i++;
            character_count++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_GTE;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else if (std::isalnum(contents.at(i)) != 0 || (std::isspace(contents.at(i)) != 0)) {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_GT;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else {
                // @todo check me for correctness
                i--;
                character_count--;
            }
        } else if (current_char == '<') {
            i++;
            character_count++;

            if (contents.at(i) == '=') {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_LTE;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else if (std::isalnum(contents.at(i)) != 0 || (std::isspace(contents.at(i)) != 0)) {
                Token token;
                token.type = TokenType::CONDITION_OPERATOR_LT;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
            } else {
                // @todo check me for correctness
                i--;
                character_count--;
            }
        } else if (current_char == '#') {
            i++;

            while (std::isalnum(contents.at(i)) != 0) {
                buffer.push_back(contents.at(i));
                i++;
                character_count++;
            }

            i--;
            character_count--;

            if (buffer == "asm") {
                Token token;
                token.type = TokenType::ASM;
                token.line = line_count;
                token.character = character_count;
                tokens.push_back(token);
                buffer.clear();
            } else {
                printf("UNSUPPORTED COMPILER DIRECTIVE, ILLEGAL!\n");
                exit(EXIT_FAILURE);
            }
        } else if (current_char == '"') {
            i++;

            while (contents.at(i) != '"') {
                buffer.push_back(contents.at(i));
                i++;
                character_count++;
            }

            Token token;
            token.type = TokenType::STRING;
            token.value = buffer;
            token.line = line_count;
            token.character = character_count - buffer.length() + 1;
            tokens.push_back(token);
            buffer.clear();
        } else if (current_char == '\n') {
            line_count++;
            character_count = 1;
            continue;
        } else if (isspace(current_char) != 0) {
            character_count++;
            continue;
        } else {
            printf("Unrecognizable character '%c' near or at %d:%d\n.", current_char, line_count, character_count);
            exit(EXIT_FAILURE);
        }
    }

    // lets push this on at the end to make life easier in the future
    // (i have no idea if this will actually bring any benefits)
    Token token;
    token.type = TokenType::_EOF;
    token.line = line_count;
    token.character = character_count;
    tokens.push_back(token);

    return tokens;
}
