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
		// escape escape sequences
		std::string ret;
		for (const char c : *str)
			if (c == '\\')	ret += "\\\\";
			else if (c == '\n')	ret += "\\n";
			else if (c == '\r')	ret += "\\r";
			else if (c == '\b')	ret += "\\b";
			else if (c == '\t') ret += "\\t";
			else if (c == '\f') ret += "\\f";
			else if (c == '\v') ret += "\\v";
			else ret += c;

		return "\"" + ret + "\"";

	} else if (type == EMT) {
		return "empty";
	} else if (type == MAC) {
		return "{" + *str + "}";
 	} else if (type == REF) {
		Value* v = defer();
		if (v)
			return v->toString();
		return "cyclic reference";
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
		Value* v = defer();
		if (v)
			return v->toString();
		return "cyclic reference";
	}

	return "idk";
}