#include <cstdio>
#include <sstream>
#include <unordered_map>

#include "parser.hpp"
#include "generator.hpp"

std::vector<std::unordered_map<std::string, Symbol>> st_stack;

// this insane hack will let us handle up to 8 variables until we start overwriting the symbol
// table locations. literally the worst solution ever but im too tired to solve it another way
// right now -_-
int current_offset = -128;

Symbol declare_variable(const std::string& name) {
    auto& current_scope = st_stack.back();

    if (current_scope.find(name) != current_scope.end()) {
        return current_scope[name];
    }

    Symbol symbol = { name, current_offset };
    current_scope[name] = symbol;

    current_offset -= 16;

    return symbol;
}

Symbol lookup_variable(const std::string& name) {
    for (int i = st_stack.size() - 1; i >= 0; --i) {
        auto current_table = st_stack.at(i);

        if (current_table.find(name) != current_table.end()) {
            return current_table[name];
        }
    }

    printf("Variable '%s' was not found in this scope.\n", name.c_str());
    exit(EXIT_FAILURE);
}

void enter_scope() {
    std::unordered_map<std::string, Symbol> this_scope;
    st_stack.push_back(this_scope);
}

void exit_scope(std::stringstream& stream) {
    auto& current_scope = st_stack.back();

    for (int i = 0; i < current_scope.size(); i++) {
        stream << "\tldr x1, [sp], 16\n";
        current_offset += 16;
    }

    st_stack.pop_back();
}

const char* condition_operator_to_arm64_condition_flag(const std::string& cond_operator)
{
    if (cond_operator == "==") {
        return "NE";
    }

    // maybe we should fail more gracefully here?
    printf("Unknown condition operator %s, cannot continue.", cond_operator.c_str());
    exit(EXIT_FAILURE);
}

int pointer = 0;

// used to calculate jump labels for jumping back into the main method
int jump_index = 0;

void generate_code(const std::shared_ptr<ASTNode>& node, std::stringstream& stream)
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
            pointer -= 16;

            generate_code(node->children[1], stream);
            stream << "\tldr x1, [sp], 16\n";
            pointer += 16;

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

            auto symbol = declare_variable(node->value);
            int memory_location = symbol.memory_location - pointer;

            stream << "\tstr x0, [sp, " << memory_location << "]\n";

            break;
        }
        case NodeType::Identifier: {
            auto symbol = lookup_variable(node->value);
            int memory_location = symbol.memory_location - pointer;

            stream << "\tldr x0, [sp, " << memory_location << "]\n";

            break;
        }
        case NodeType::Boolean: {
            int bool_value = node->value == "true" ? 1 : 0;

            stream << "\tmov x0, #" << bool_value << "\n";

            break;
        }
        case NodeType::ConditionOperator: {
            generate_code(node->children[0], stream);
            stream << "\tstr x0, [sp, -16]!\n";
            pointer -= 16;

            generate_code(node->children[1], stream);
            stream << "\tldr x1, [sp], 16\n";
            pointer += 16;

            stream << "\tcmp x0, x1\n";

            break;
        }
        case NodeType::If: {
            std::shared_ptr<ASTNode> expression_node = node->children.at(0);

            generate_code(expression_node, stream); // expression

            if (expression_node->type == NodeType::ConditionOperator) {
                // based on comparison operator we select a condition flag, e.g. == becomes EQ
                const char* condition_flag = condition_operator_to_arm64_condition_flag(expression_node->value);

                // maybe calculate label at lexer time, and put it in the value for the if node?
                // store jump labels somewhere (symbol table?) or above method
                stream << "\tb." << condition_flag << " _else" << jump_index << "\n";
            } else if (expression_node->type == NodeType::Boolean) {
                stream << "\tcmp x0, #1\n";

                stream << "\tb.ne _else" << jump_index << "\n";

                // we should be able to fold at least one branch here no?
            }

            stream << "_if" << jump_index << ":\n";
            generate_code(node->children[1], stream);

            if (node->children.at(node->children.size() - 1)->type == NodeType::Else) {
                generate_code(node->children[node->children.size() - 1], stream);
            } else {
                // generate an empty else for current jump index as to not fall through
                stream << "_else" << jump_index << ":\n";
            }

            stream << "_main_" << jump_index << ":\n";

            jump_index++;

            break;
        }
        case NodeType::Else: {
            stream << "_else" << jump_index << ":\n";

            generate_code(node->children[0], stream);

            break;
        }
        case NodeType::Block: {
            enter_scope();

            for (int i = 0; i < node->children.size(); i++) {
                generate_code(node->children[i], stream);
            }

            exit_scope(stream);

            stream << "\tb _main_" << jump_index << "\n";

            break;
        }
        case NodeType::Directive: {
            std::string directive_type = node->value;

            if (directive_type == "asm") {
                for (int i = 0; i < node->children[0]->children.size(); i++) {
                    // lets hops the user knows assembly :)
                    // also, indent this depending on scope, maybe??
                    stream << "\t" << node->children[0]->children[i]->value << "\n";
                }
            }

            break;
        }
        case NodeType::String: {
            // @todo handle string case
            break;
        }
    }
}


void generate(const std::shared_ptr<ASTNode>& node, std::stringstream& stream) {
    pointer = 0;
    jump_index = 0;

    std::unordered_map<std::string, Symbol> global_scope;

    st_stack.push_back(global_scope);

    generate_code(node, stream);
}
