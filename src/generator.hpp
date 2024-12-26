#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "parser.hpp"

struct Symbol {
    std::string name;
    int memory_location;
};

void generate(const std::shared_ptr<ASTNode>& node, std::stringstream& stream);

#endif
