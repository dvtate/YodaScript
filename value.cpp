#include <cstdio>
#include <sstream>
#include <string>

#include "value.hpp"
#include "namespace.hpp"


Value::Value():
		type(Value::EMT) {}
Value::Value(const vtype t): type(t) {}
Value::Value(const vtype t, const std::string v):
		type(t), str(new std::string(v)) {}
Value::Value(const char* v):
		type(Value::STR), str(new std::string(v)) {}
Value::Value(const std::string& v):
		type(Value::STR), str(new std::string(v)) {}
Value::Value(const double v):
		type(Value::DEC), dec(v) {}
Value::Value(std::shared_ptr<Value> ref):
	type(Value::REF), ref(new std::shared_ptr<Value>(ref)) {}
Value::Value(const vtype t, const std::shared_ptr<Value>& ref):
		type(t), ref(new std::shared_ptr<Value>(ref)) {}
Value::Value(mpz_class mp_integer):
	type(Value::INT), mp_int(new mpz_class(mp_integer)) {}
Value::Value(const std::vector<Value>& v):
		type(Value::ARR), arr(new std::vector<Value>(v)) {}
Value::Value(const nullptr_t& null):
		type(REF), ref(new std::shared_ptr<Value>(nullptr)) {}
Value::Value(const Def& def):
		type(DEF), def(new Def(def)) {}
Value::Value(const Namespace& ns):
		type(NSP), ns(new Namespace(ns)) {}

void Value::erase() {
	// only data on heap needs to be deleted
	if (type < Value::STR)
		return;
	if (type == Value::STR || type == Value::MAC)
		delete str;
	if (type == Value::INT)
		delete mp_int;
	if (type == REF || type == IMR)
		delete ref;
	if (type == Value::ARR)
		delete arr;
	if (type == Value::NSP)
		delete ns;
	if (type == Value::DEF)
		delete def;
}

inline Value& Value::set_noerase(const Value& v) {
	type = v.type;

	if (type == DEC) {
		dec = v.dec;
	} else if (type == REF || type == IMR) {
		ref = new std::shared_ptr<Value>();
		*ref = *v.ref;
	} else if (type == STR || type == MAC) {
		str = new std::string(*v.str);
	} else if (type == INT) {
		mp_int = new mpz_class(*v.mp_int);
	} else if (type == ARR) {
		arr = new std::vector<Value>(*v.arr);
	}
	return *this;
}

inline static size_t countLines(const std::string& s)
{
	size_t lc = 0;
	for (const char c : s)
		if (c == '\n')
			lc++;
	return lc;
}

std::string Value::repr() {
	if (type == DEC) {
		// add .0 for whole numbers
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
	} else if (type == REF || type == IMR) {

		Value* v = (Value*) defer();
		if (v)
			return v->repr();
		return "cyclic/null reference";
	} else if (type == ARR) {
		std::string ret = "(";
		for (Value& v : *arr)
			ret += v.repr() + ", ";
		ret[ret.length() - 1] = ')';
		return ret;
	}

	return "idk";
}

std::string Value::toString() {
	if (type == DEC) {
		std::ostringstream ss;
		ss << dec;
		return ss.str();
	} else if (type == INT) {
		std::ostringstream ss;
		ss << *mp_int;
		return ss.str();
	} else if (type == STR || type == MAC) {
		return *str;
	} else if (type == EMT) {
		return "empty";
	} else if (type == REF || type == IMR) {
		Value* v = (Value*) defer();
		if (v)
			return v->toString();
		return "cyclic/null reference";
	} else if (type == ARR) {
		std::string ret = "(";
		for (Value& v : *arr)
			ret += v.repr() + ", ";
		ret[ret.length() - 1] = ')';
		return ret;
	}

	return "idk";
}


// get the value that a reference points to
const Value* Value::defer(std::vector<std::shared_ptr<Value>*> pastPtrs) {
	// end of ref recursion
	if (type != REF && type != IMR)
		return this;
	if (!ref)
		return nullptr;

	// if it's been seen before it should be cyclic reference
	if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
		return nullptr;

	// follow reference tree
	pastPtrs.emplace_back(ref);
	if (!ref || !*ref)
		return nullptr;
	return (*ref)->defer(pastPtrs);
}

// get muteable value
// stops at immuteable references
Value* Value::deferMuteable(std::vector<std::shared_ptr<Value>*> pastPtrs) {
	// end of ref recursion
	if (type != REF) // will return IMR
		return this;
	if (!ref)
		return nullptr;

	// if it's been seen before it should be cyclic reference
	if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
		return nullptr;

	// follow reference tree
	pastPtrs.emplace_back(ref);
	if (!ref || !*ref)
		return nullptr;
	return (*ref)->deferMuteable(pastPtrs);
}

const char* Value::typeName() {
	// could be optimized to an array of strings
	// and using type as index, but im not certain if i can keep the values in correct order
	switch (type) {
		case EMT: return "empty";
		case INT: return "int";
		case DEC: return "float";
		case REF: return "reference";
		case IMR: return "const_ref";
		case STR: return "string";
		case MAC: return "macro";
		case ARR: return "list";
		case OBJ: return "object";
		case LAM: return "lambda";

		default:
			std::cout <<"unknown type#" <<(const long)type <<std::endl;
			return "unknown";
	}
}

bool Value::truthy() {
	if (type == EMT)	return false;
	if (type == REF)	return defer();
	if (type == INT)	return *mp_int != 0;
	if (type == DEC)	return dec != 0;
	if (type == STR)	return (bool) str->length();
	if (type == ARR)	return !arr->empty();
	if (type == OBJ || type == MAC || type == LAM)
		return true;

	std::cout <<"invalid type in Value.truthy()\n";
	// glitch
	return false;
}

// should i?
bool Value::operator==(Value& v) {
	if (v.type != type)
		return false;
	if (type == EMT)
		return true;
	if (type == INT)
		return *v.mp_int == *mp_int;
	if (type == DEC)
		return v.dec == dec;
	if (type == STR || type == MAC)
		return *v.str == *str;

	// returns if they reference same data,
	// use copy operator if u wanna compare by value
	// TODO: move this to an `is` operator
	if (type == REF)
		return v.defer() == defer();

	// for now at least
	//if (type == LAM || type == OBJ)
	return false;
}