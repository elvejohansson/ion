#include <cstdio>
#include <sstream>
#include <string>
#include <unordered_map>

#include "parser.hpp"

std::unordered_map<std::string, int> symbol_table;

// this insane hack will let us handle up to 8 variables until we start overwriting the symbol
// table locations. literally the worst solution ever but im too tired to solve it another way
// right now -_-
int current_offset = -128;

int get_variable_offset(const std::string& variable_name, int stack_pointer) {

    if (symbol_table.find(variable_name) == symbol_table.end()) {
        symbol_table[variable_name] = current_offset;
        current_offset -= 16;
    }

    return symbol_table[variable_name] - stack_pointer;
}

const char* condition_operator_to_arm64_condition_flag(const std::string &cond_operator)
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

            std::string variable_name = node->value;
            int stack_offset = get_variable_offset(variable_name, pointer);

            stream << "\tstr x0, [sp, " << stack_offset << "]\n";

            break;
        }
        case NodeType::Identifier: {
            std::string variable_name = node->value;
            int stack_offset = get_variable_offset(variable_name, pointer);

            stream << "\tldr x0, [sp, " << stack_offset << "]\n";

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
            // @todo handle scopes

            for (int i = 0; i < node->children.size(); i++) {
                generate_code(node->children[i], stream);
            }

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
