//
// Created by tate on 04-03-19.
//

#ifndef YS2_OBJECT_HPP
#define YS2_OBJECT_HPP

#include <string>
#include <memory>
#include <unordered_map>


#include "namespace_def.hpp"

class Value;

class Object {
public:
	std::unordered_map<std::string, std::shared_ptr<Value>> members;
	std::shared_ptr<Value>& getMember(const std::string& name);
};


#endif //YS2_OBJECT_HPP
