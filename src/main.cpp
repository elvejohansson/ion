#include <fstream>
#include <sstream>

#include "lexer.hpp"
#include "parser.hpp"
#include "generator.hpp"

#if defined (__APPLE__)
    #define _MAC_OS
#endif

#if defined (__aarch64__)
    #define _ARM64
#endif

void compile_program(const std::stringstream& stream)
{
    std::ofstream program;
    program.open("./build/program.s");
    program << stream.str();
    program.close();

#if defined (_MAC_OS) && defined (_ARM64)
    std::system("as ./build/program.s -o ./build/program.o");
    std::system("ld ./build/program.o -o ./build/program -e _main");
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

    auto frontend_start = std::chrono::high_resolution_clock::now();

    std::vector<Token> tokens = tokenize(contents);

    auto ast_root_node = parse(&tokens);

    auto frontend_elapsed = std::chrono::high_resolution_clock::now() - frontend_start;

    auto backend_start = std::chrono::high_resolution_clock::now();
    std::stringstream buffer;
    buffer << ".global _main\n";
    buffer << ".text\n";
    buffer << "\n_main:\n";

    generate(ast_root_node, buffer);

    auto backend_elapsed = std::chrono::high_resolution_clock::now() - backend_start;

    compile_program(buffer);

    printf("\n");
    printf("Front end took %lld μs\n", std::chrono::duration_cast<std::chrono::microseconds>(frontend_elapsed).count());
    printf("Back end took %lld μs\n", std::chrono::duration_cast<std::chrono::microseconds>(backend_elapsed).count());

    return 0;
}
