#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

enum TokenType : uint8_t {
    INT_LIT,         // 123
    IDENTIFIER,      // xy
    ASSIGNMENT,      // =

    BOOLEAN,         // true / false

    IF,              // if
    ELSE,            // else

    OPERATOR_PLUS,   // +
    OPERATOR_MINUS,  // -
    OPERATOR_STAR,   // *
    OPERATOR_SLASH,  // /

    CONDITION_OPERATOR_EQ,  // ==

    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }

    _EOF,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int character;
};

const char* print_token_type(TokenType token_type);

std::vector<Token> tokenize(std::string contents);

#endif
