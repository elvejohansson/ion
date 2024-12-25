#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "parser.hpp"

int get_variable_offset(const std::string& variable_name, int stack_pointer);

const char* condition_operator_to_arm64_condition_flag(const std::string cond_operator);

void generate_code(const std::shared_ptr<ASTNode> node, std::stringstream& stream);

#endif
