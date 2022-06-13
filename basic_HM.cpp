#include <cstdlib>
#include "string"
#include "vector"
#include "map"
#include "set"

#include "basic_HM.h"


TypeVariable::TypeVariable() = default;


TypeOperator::TypeOperator (
    const std::string& name,
    const std::vector<TypeVariable*>& types
) {
    this->name = name;
    this->types = types;
}



bool is_number(const std::string& name) {
    char *p;
    strtol(name.c_str(), &p, 10);

    return !*p;
}


Node* get_base_type(Node* object) {
    auto* type_var = dynamic_cast<TypeVariable*>(object);

    if (type_var != nullptr) {
        if (type_var->instance != nullptr) {
            type_var->instance = get_base_type(type_var->instance);
            return type_var->instance;
        }
    }

    return object;
}


bool any_type_match(
    Node* target,
    const std::vector<TypeVariable*>& source
) {
    for (auto& type : source) {
        bool is_match = type_match(target, type);
        if (is_match) {
            return true;
        }
    }

    return false;
}


bool type_match(
    Node* target,
    Node* source
) {
    Node* source_base_type = get_base_type(source);

    if (source_base_type == target) {
        return true;
    } else {
        auto* type_op = dynamic_cast<TypeOperator*>(source_base_type);
        if (type_op != nullptr) {
            return any_type_match(target, type_op->types);
        }
        return false;
    }
}


bool is_generic_type(
    Node* target,
    const std::vector<TypeVariable *> &source
) {
    return !any_type_match(target, source);
}


void unification(
    Node* type1,
    Node* type2
) {
    Node* base_type_1 = get_base_type(type1);
    Node* base_type_2 = get_base_type(type2);

    auto* type_var1 = dynamic_cast<TypeVariable*>(base_type_1);
    auto* type_var2 = dynamic_cast<TypeVariable*>(base_type_1);
    auto* type_op1 = dynamic_cast<TypeOperator*>(base_type_1);
    auto* type_op2 = dynamic_cast<TypeOperator*>(base_type_1);

    if (type_var1 != nullptr) {
        if (type_var1 != base_type_2) {
            if (type_match(type_var1, base_type_2)) {
                throw std::invalid_argument(
                        "Cannot unify recursively: " + type_var1->toString() + ", " + base_type_2->toString()
                );
            }
            type_var1->instance = base_type_2;
        }
    } else {
        if (type_op1 != nullptr && type_var2 != nullptr) {
            unification(type_var2, type_op1);
        } else {
            if (type_op1 != nullptr && type_op2 != nullptr) {
                if (type_op1->name != type_op2->name || type_op1->types.size() != type_op2->types.size()) {
                    throw std::invalid_argument(
                            "type mismatch: " + type_op1->toString() + " != " + type_op2->toString()
                    );
                }

                for (int i = 0; i < std::min(type_op1->types.size(), type_op2->types.size()); ++i) {
                    unification(type_op1->types[i], type_op2->types[i]);
                }
            } else {
                throw std::invalid_argument(
                        "cannot unify: " + type1->toString() + " and " + type2->toString()
                );
            }
        }
    }
}


Node* copy_type_rec(
    Node* type,
    const std::vector<TypeVariable *> &non_generic,
    std::map<TypeVariable*, TypeVariable*> mapping
) {
    Node* base_type = get_base_type(type);

    auto* type_var = dynamic_cast<TypeVariable*>(base_type);
    auto* type_op = dynamic_cast<TypeOperator*>(base_type);

    if (type_var != nullptr) {
        if (is_generic_type(type_var, non_generic)) {

            if (mapping.count(type_var) == 0) {
                mapping.emplace(type_var, new TypeVariable());
            }

            return mapping[type_var];
        } else {
            return type_var;
        }
    } else {
        if (type_op != nullptr) {
            std::vector<TypeVariable*> new_types;

            for (auto& cur_type : type_op->types) {
                Node* copied_obj = copy_type_rec(cur_type, non_generic, mapping);
                auto* copied_type = dynamic_cast<TypeVariable*>(copied_obj);

                if (copied_type != nullptr) {
                    new_types.push_back(copied_type);
                } else {
                    throw std::invalid_argument("Not a variable: " + copied_obj->toString());
                }
            }

            return new TypeOperator(type_op->name, new_types);
        } else {
            throw std::invalid_argument("Not a variable or operator: " + base_type->toString());
        }
    }
}


Node* copy_type(
    Node* type,
    const std::vector<TypeVariable *> &non_generic
) {
    std::map<TypeVariable*, TypeVariable*> mapping;

    return copy_type_rec(type, non_generic, mapping);
}


auto global_idents = name_table();
auto global_funcs = std::map<std::string, std::pair<Node*, std::vector<std::pair<std::string, Value>>>>();

std::pair<Value, name_table> analyse(
    Node *node,
    bool inside_func_or_block,
    name_table local_vars,
    bool is_usub
) {
    Tag& current_tag = node->get_tag();

    if (current_tag == Tag::NUMBER) {
        double val = std::stod(node->get_label());

        if (is_usub) {
            val = -val;
        }

        return {{val, Value::dimensionless}, local_vars};
    }

    if (current_tag == Tag::IDENT) {
        auto& ident_name = node->get_label();

        if (global_idents.count(ident_name) > 0) {
            return {global_idents[ident_name], local_vars};
        } else if (inside_func_or_block && local_vars.count(ident_name) > 0) {
            return {local_vars[ident_name], local_vars};
        } else {
            throw std::invalid_argument("IDENT does not exists; node: " + node->toString());
        }
    }

    if (current_tag == Tag::FUNC) {
        bool is_present = false;
        auto args = name_table();

        for (const auto& elem : global_funcs) {
            if (elem.first == node->get_label()) {
                is_present = true;

                for (int i = 0; i < node->fields.size(); ++i) {
                    args.emplace(
                        elem.second.second[i].first,
                        analyse(
                            node->fields[i],
                            inside_func_or_block,
                            local_vars,
                            is_usub
                        ).first
                    );
                }

                return analyse(
                    elem.second.first,
                    true,
                    args,
                    is_usub
                );
            }
        }

        throw std::invalid_argument("FUNC does not exists; node: " + node->toString());
    }

    if (current_tag == Tag::BEGINC) {
        for (auto option : node->fields) {
            if (option->cond != nullptr) {
                Tag cond_tag = option->cond->get_tag();

                auto left = analyse(option->cond->left, inside_func_or_block, local_vars, is_usub);
                auto right = analyse(option->cond->right, inside_func_or_block, left.second, is_usub);

                if (!(
                        left.first._type == right.first._type &&
                        left.first._type == Value::DOUBLE &&
                        Value::check_dimensions(left.first.get_dimension(), right.first.get_dimension())
                )) {
                    throw std::invalid_argument(
                            "Cannot compare non double (or with different dimension) value: " +
                            to_string(left.first) +
                            " and value: " +
                            to_string(right.first)
                    );
                }

                switch (cond_tag) {
                    case Tag::GT:
                        if (left.first.get_double() > right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    case Tag::GEQ:
                        if (left.first.get_double() >= right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    case Tag::LT:
                        if (left.first.get_double() < right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    case Tag::LEQ:
                        if (left.first.get_double() <= right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    case Tag::EQ:
                        if (left.first.get_double() == right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    case Tag::NEQ:
                        if (left.first.get_double() != right.first.get_double()) {
                            return analyse(option->right, inside_func_or_block, right.second, is_usub);
                        } else {
                            continue;
                        }
                    default:
                        throw std::invalid_argument(
                                "Cannot compare with wrong condition tag: " +
                                to_string(cond_tag) +
                                " value: " +
                                to_string(left.first) +
                                " and value: " +
                                to_string(right.first)
                        );
                }
            } else {
                return analyse(option->right, inside_func_or_block, local_vars, is_usub);
            }
        }
    }

    if (current_tag == Tag::UADD || current_tag == Tag::NOT) {
        return analyse(node->right, inside_func_or_block, local_vars, is_usub);
    }

    if (current_tag == Tag::USUB) {
        return analyse(node->right, inside_func_or_block, local_vars, true);
    }

    if (
        current_tag == Tag::ADD ||
        current_tag == Tag::SUB ||
        current_tag == Tag::AND ||
        current_tag == Tag::OR
    ) {
        auto left = analyse(node->left, inside_func_or_block, local_vars, is_usub);
        auto right = analyse(node->right, inside_func_or_block, left.second, is_usub);

        if (!(
            left.first._type == right.first._type &&
            left.first._type == Value::DOUBLE &&
            Value::check_dimensions(left.first.get_dimension(), right.first.get_dimension())
        )) {
            throw std::invalid_argument(
                    "Cannot ADD/SUB non double (or with different dimension) value: " +
                    to_string(left.first) +
                    " and value: " +
                    to_string(right.first)
            );
        }

        return right;
    }

    if (current_tag == Tag::MUL || current_tag == Tag::DIV || current_tag == Tag::FRAC) {
        auto left = analyse(node->left, inside_func_or_block, local_vars, is_usub);
        auto right = analyse(node->right, inside_func_or_block, left.second, is_usub);

        if (!(
                left.first._type == right.first._type &&
                left.first._type == Value::DOUBLE
        )) {
            throw std::invalid_argument(
                    "Cannot MUL/DIV/FRAC non double value: " +
                    to_string(left.first) +
                    " and value: " +
                    to_string(right.first)
            );
        }

        if (current_tag == Tag::MUL) {
            return {
                {Value::sum_dimensions(left.first.get_dimension(), right.first.get_dimension())},
                right.second
            };
        } else {
            return {
                {Value::sub_dimensions(left.first.get_dimension(), right.first.get_dimension())},
                right.second
            };
        }
    }

    if (current_tag == Tag::POW) {
        auto left = analyse(node->left, inside_func_or_block, local_vars, is_usub);
        auto right = analyse(node->right, inside_func_or_block, left.second, is_usub);

        if (!(
                left.first._type == right.first._type &&
                left.first._type == Value::DOUBLE &&
                Value::is_dimensionless(right.first) &&
                right.first.get_double() == trunc(right.first.get_double()) &&
                right.first.get_double() >= 1
        )) {
            throw std::invalid_argument(
                    "Cannot POW non double (or to dimensional or non integer degree) value: " +
                    to_string(left.first) +
                    " and value: " +
                    to_string(right.first)
            );
        }

        return {
            {Value::mul_dimensions(left.first.get_dimension(), (int) right.first.get_double())},
            right.second
        };
    }

    if (current_tag == Tag::SUM || current_tag == Tag::PRODUCT) {
        auto left = analyse(node->left, inside_func_or_block, local_vars, is_usub);
        auto cond = analyse(node->cond, inside_func_or_block, left.second, is_usub);
        auto right = analyse(node->right, inside_func_or_block, cond.second, is_usub);

        if (!(
            left.first._type == right.first._type &&
            left.first._type == Value::DOUBLE &&
            Value::is_dimensionless(left.first) &&
            Value::is_dimensionless(cond.first)
        )) {
            throw std::invalid_argument(
                    "Cannot use SUM operator on non double (or on dimensional) value: " +
                    to_string(left.first) +
                    " and value: " +
                    to_string(cond.first)
            );
        }

        if (current_tag == Tag::SUM) {
            return analyse(node->right, inside_func_or_block, local_vars, is_usub);
        } else {
            return {
                Value::mul_dimensions(
                    right.first.get_dimension(),
                    floor(cond.first.get_double() - left.first.get_double())
                ),
                right.second
            };
        }
    }

    if (current_tag == Tag::DIMENSION) {
        return {
            {dimensions.find(node->get_label())->second},
            local_vars
        };
    }

    if (current_tag == Tag::ABS) {
        auto right = analyse(node->right, inside_func_or_block, local_vars, is_usub);

        if (right.first._type != Value::DOUBLE) {
            throw std::invalid_argument(
                    "Cannot use ABS operator on non double value: " +
                    to_string(right.first)
            );
        }

        return right;
    }

    if (current_tag == Tag::EQ) {
        if (node->right->get_tag() != Tag::PLACEHOLDER) {
            return analyse(node->right, inside_func_or_block, local_vars, is_usub);
        } else {
            return analyse(node->left, inside_func_or_block, local_vars, is_usub);
        }
    }

    if (current_tag == Tag::SET) {
        if (inside_func_or_block) {
            local_vars.emplace(
                 node->left->get_label(),
                 analyse(node->right, inside_func_or_block, local_vars, is_usub).first
             );

            return {
                Value(),
                local_vars
            };
        } else {
            if (node->left->get_tag() == Tag::IDENT) {
                global_idents.emplace(
                    node->left->get_label(),
                    analyse(node->right, inside_func_or_block, local_vars, is_usub).first
                );

                return {
                    Value(),
                    local_vars
                };
            } else if (node->left->get_tag() == Tag::FUNC) {
                auto res = std::vector<std::pair<std::string, Value>>();
                for (auto field : node->left->fields) {
                    res.emplace_back(field->get_label(), Value());
                }

                global_funcs.emplace(
                    node->left->get_label(),
                    std::pair<Node*, std::vector<std::pair<std::string, Value>>>(node->right, res)
                );

                return {
                    Value(),
                    local_vars
                };
            } else {
                throw std::invalid_argument("Cannot analyse SET statement: " + node->toString());
            }
        }
    }

    if (current_tag == Tag::BEGINB) {
        for (int i = 0; i < node->fields.size(); ++i) {
            if (i == node->fields.size() - 1) {
                return analyse(node->fields[i], true, local_vars, is_usub);
            } else {
                auto res = analyse(node->fields[i], true, local_vars, is_usub);
                local_vars = res.second;
            }
        }
    }

    if (current_tag == Tag::BEGINM) {
        return {Value(Matrix()), local_vars};
    }

    throw std::invalid_argument("Cannot analyse node: " + node->toString());
}