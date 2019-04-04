//
// Created by tate on 04-03-19.
//

#ifndef YS2_OBJECT_HPP
#define YS2_OBJECT_HPP

#include <string>
#include <memory>
#include <unordered_map>


#include "extend.hpp"

class Value;
class Frame;
class Exit;

// internal implementation of the object datatype
class Object {
public:

	std::unordered_map<std::string, std::shared_ptr<Value>> members;

	std::shared_ptr<Value>& getMember(const std::string& name);

	bool callMember(Frame& f, const std::string& name, Exit& ev);
	bool callMember(Frame& f, const std::string& name, Exit& ev, const std::shared_ptr<Value>& obj, const size_t args = 0);
};


#endif //YS2_OBJECT_HPP
