#include <cstdio>
#include <sstream>
#include <string>

#include "value.hpp"

std::string Value::repr()
{
	if (type == NUM) {
		std::stringstream ss;
		ss<<num;
		return ss.str();

	} else if (type == STR) {
		return "\"" + *str + "\"";
	} else if (type == EMT) {
		return "empty";
	} else if (type == MAC) {
		return "{" + *str + "}";
	}

	return "idk";
}

std::string Value::toString()
{
	if (type == NUM) {
		std::stringstream ss;
		ss<<num;
		return ss.str();
	} else if (type == STR) {
		return *str;
	} else if (type == EMT) {
		return "empty";
	}

	return "idk";
}