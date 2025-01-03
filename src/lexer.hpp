#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

enum TokenType : uint8_t {
    INT_LIT,         // 123
    IDENTIFIER,      // xy
    ASSIGNMENT,      // =
    BANG,            // !

    BOOLEAN,         // true / false
    STRING,          // "hello"

    IF,              // if
    ELSE,            // else

    OPERATOR_PLUS,   // +
    OPERATOR_MINUS,  // -
    OPERATOR_STAR,   // *
    OPERATOR_SLASH,  // /

    CONDITION_OPERATOR_EQ,  // ==
    CONDITION_OPERATOR_NE,  // !=
    CONDITION_OPERATOR_GT,  // >
    CONDITION_OPERATOR_LT,  // <
    CONDITION_OPERATOR_GTE, // >=
    CONDITION_OPERATOR_LTE, // <=

    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }

    ASM, // #asm

    _EOF,
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int character;
};

const char* print_token_type(TokenType token_type);

std::vector<Token> tokenize(const std::string& contents);

#endif
