#include <cstdio>
#include <fstream>
#include <sstream>

typedef enum TokenType {
    INT_LIT,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
} TokenType;

typedef struct Token {
    TokenType type;
    std::string value;
} Token;

std::string token_format(TokenType token_type)
{
    switch (token_type) {
        case INT_LIT: return "INT_LIT";
        case OPERATOR_PLUS: return "OPERATOR_PLUS";
        case OPERATOR_MINUS: return "OPERATOR_MINUS";
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

    for (int i = 0; i < tokens.size(); i++) {
        printf("%d: %s %s\n", i, token_format(tokens[i].type).c_str(), tokens[i].value.c_str());
    }

    return 0;
}
