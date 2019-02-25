#include <cstdio>
#include <sstream>
#include <string>

#include "value.hpp"

std::string Value::repr()
{
	if (type == DEC) {
		std::ostringstream ss;
		ss << dec;
		return ss.str();
	} else if (type == INT) {
		std::ostringstream ss;
		ss << *mp_int;
		return ss.str();
	} else if (type == STR) {
		return "\"" + *str + "\"";
	} else if (type == EMT) {
		return "empty";
	} else if (type == MAC) {
		return "{" + *str + "}";
 	} else if (type == REF) {
		return defer().toString();
	}

	return "idk";
}

std::string Value::toString()
{
	if (type == DEC) {
		std::ostringstream ss;
		ss << dec;
		return ss.str();
	} else if (type == INT) {
		std::ostringstream ss;
		ss << *mp_int;
		return ss.str();
	} else if (type == STR) {
		return *str;
	} else if (type == EMT) {
		return "empty";
	} else if (type == MAC) {
		return "{" + *str + "}";
	} else if (type == REF) {
		return defer().toString();
	}

	return "idk";
}