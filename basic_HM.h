//#pragma once
//
//#include <cstdlib>
//#include <algorithm>
//#include <utility>
//#include "string"
//#include "vector"
//#include "map"
//#include "set"
//
//
//class TypeVariable: public Node {
//public:
//    Node* instance;
//
//    TypeVariable();
//};
//
//
//class TypeOperator: public Node {
//public:
//    std::string name;
//    std::vector<TypeVariable*> types;
//
//    TypeOperator(const std::string& name, const std::vector<TypeVariable*>& types);
//};
//
//
//bool is_number(const std::string& name);
//
//Node* get_base_type(Node* object);
//
//bool any_type_match(Node* target, std::vector<TypeVariable*> source);
//
//bool type_match(Node* target, Node* source);
//
//bool is_generic_type(Node* target, const std::set<TypeVariable*>& source);
//
//void unification(Node* type1, Node* type2);
//
//Node* copy_type_rec(
//    Node* type,
//    const std::set<TypeVariable*>& non_generic,
//    std::map<TypeVariable*, TypeVariable*> mapping
//);
//
//Node* copy_type(Node* type, const std::set<TypeVariable*>& non_generic);
//
//Value& analyse(Node* node);