//#include <cstdlib>
//#include <algorithm>
//#include <utility>
//#include "string"
//#include "vector"
//#include "map"
//#include "set"
//
//#include "basic_HM.h"
//
//
//TypeVariable::TypeVariable() = default;
//
//
//TypeOperator::TypeOperator(
//    const std::string& name,
//    const std::vector<TypeVariable*>& types
//) {
//    this->name = name;
//    this->types = types;
//}
//
//
//
//bool is_number(const std::string& name) {
//    char *p;
//    strtol(name.c_str(), &p, 10);
//
//    return !*p;
//}
//
//
//Node* get_base_type(Node* object) {
//    auto* type_var = dynamic_cast<TypeVariable*>(object);
//
//    if (type_var != nullptr) {
//        if (type_var->instance != nullptr) {
//            type_var->instance = get_base_type(type_var->instance);
//            return type_var->instance;
//        }
//    }
//
//    return object;
//}
//
//
//bool any_type_match(
//        Node* target,
//        const std::set<TypeVariable*>& source
//) {
//    for (auto& type : source) {
//        bool is_match = type_match(target, type);
//        if (is_match) {
//            return true;
//        }
//    }
//
//    return false;
//}
//
//
//bool type_match(
//        Node* target,
//        Node* source
//) {
//    Node* source_base_type = get_base_type(source);
//
//    if (source_base_type == target) {
//        return true;
//    } else {
//        auto* type_op = dynamic_cast<TypeOperator*>(source_base_type);
//        if (type_op != nullptr) {
//            return any_type_match(target, type_op->types);
//        }
//        return false;
//    }
//}
//
//
//bool is_generic_type(
//        Node* target,
//        const std::set<TypeVariable*>& source
//) {
//    return !any_type_match(target, source);
//}
//
//
//void unification(
//        Node* type1,
//        Node* type2
//) {
//    Node* base_type_1 = get_base_type(type1);
//    Node* base_type_2 = get_base_type(type2);
//
//    auto* type_var1 = dynamic_cast<TypeVariable*>(base_type_1);
//    auto* type_var2 = dynamic_cast<TypeVariable*>(base_type_1);
//    auto* type_op1 = dynamic_cast<TypeOperator*>(base_type_1);
//    auto* type_op2 = dynamic_cast<TypeOperator*>(base_type_1);
//
//    if (type_var1 != nullptr) {
//        if (type_var1 != base_type_2) {
//            if (type_match(type_var1, base_type_2)) {
//                throw std::invalid_argument(
//                        "Cannot unify recursively: " + type_var1->toString() + ", " + base_type_2->toString()
//                );
//            }
//            type_var1->instance = base_type_2;
//        }
//    } else {
//        if (type_op1 != nullptr && type_var2 != nullptr) {
//            unification(type_var2, type_op1);
//        } else {
//            if (type_op1 != nullptr && type_op2 != nullptr) {
//                if (type_op1->name != type_op2->name || type_op1->types.size() != type_op2->types.size()) {
//                    throw std::invalid_argument(
//                            "type mismatch: " + type_op1->toString() + " != " + type_op2->toString()
//                    );
//                }
//
//                for (int i = 0; i < std::min(type_op1->types.size(), type_op2->types.size()); ++i) {
//                    unification(type_op1->types[i], type_op2->types[i]);
//                }
//            } else {
//                throw std::invalid_argument(
//                        "cannot unify: " + type1->toString() + " and " + type2->toString()
//                );
//            }
//        }
//    }
//}
//
//
//Node* copy_type_rec(
//        Node* type,
//        const std::set<TypeVariable*>& non_generic,
//        std::map<TypeVariable*, TypeVariable*> mapping
//) {
//    Node* base_type = get_base_type(type);
//
//    auto* type_var = dynamic_cast<TypeVariable*>(base_type);
//    auto* type_op = dynamic_cast<TypeOperator*>(base_type);
//
//    if (type_var != nullptr) {
//        if (is_generic_type(type_var, non_generic)) {
//
//            if (mapping.count(type_var) == 0) {
//                mapping.insert(type_var, new TypeVariable());
//            }
//
//            return mapping[type_var];
//        } else {
//            return type_var;
//        }
//    } else {
//        if (type_op != nullptr) {
//            std::vector<TypeVariable*> new_types;
//
//            for (auto& cur_type : type_op->types) {
//                Node* copied_obj = copy_type_rec(cur_type, non_generic, mapping);
//                auto* copied_type = dynamic_cast<TypeVariable*>(copied_obj);
//
//                if (copied_type != nullptr) {
//                    new_types.push_back(copied_type);
//                } else {
//                    throw std::invalid_argument("Not a variable: " + copied_obj->toString());
//                }
//            }
//
//            return new TypeOperator(type_op->name, new_types);
//        } else {
//            throw std::invalid_argument("Not a variable or operator: " + base_type->toString());
//        }
//    }
//}
//
//
//Node* copy_type(
//        Node* type, const std::set<TypeVariable*>& non_generic
//) {
//    std::map<TypeVariable*, TypeVariable*> mapping;
//
//    return copy_type_rec(type, non_generic, mapping);
//}
//
//
//Value& analyse(
//        Node* node
//) {
//    Tag& current_tag = node->get_tag();
//
//    if (current_tag == Tag::IDENT) {
//        if (Node::global.count(node->get_label())) {
//            return Node::global[node->get_label()];
//        } else {
//            throw std::invalid_argument("IDENT does not exists; node: " + node->toString());
//        }
//    }
//
//    if (current_tag == Tag::FUNC) {
//        std::vector<std::string> args_names = {};
//        if (Node::global.count(node->get_label())) {
//            Node::global[node->get_label()].
//                    ;
//        } else {
//            throw std::invalid_argument("FUNC does not exists; node: " + node->toString());
//        }
//    }
//
//    throw std::invalid_argument("Cannot analyse node: " + node->toString());
//}