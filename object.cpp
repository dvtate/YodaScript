//
// Created by tate on 04-03-19.
//
#include "value.hpp"
#include "object.hpp"


std::shared_ptr<Value> Object::getMember(const std::string &name) {
	auto m = members.find(name);
	if (m == members.end()) {
		members.emplace(name, std::make_shared<Value>());
		m = members.find(name);
	}
	return m->second;
}