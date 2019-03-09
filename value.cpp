#include <cstdio>
#include <sstream>
#include <string>

#include "value.hpp"
#include "namespace_def.hpp"

// constructors
Value::Value():
	type(Value::EMT) {}
Value::Value(const vtype t):
	type(t)		{}
Value::Value(const vtype t, const std::string v):
	type(t),	str(new std::string(v)) {}
Value::Value(const char* v):
	type(STR),	str(new std::string(v)) {}
Value::Value(const std::string& v):
	type(STR),	str(new std::string(v)) {}
Value::Value(const double v):
	type(DEC),	dec(v) {}
Value::Value(std::shared_ptr<Value> ref):
	type(REF),	ref(new std::shared_ptr<Value>(ref)), related() {}
Value::Value(const vtype t, const std::shared_ptr<Value>& ref):
	type(t),	ref(new std::shared_ptr<Value>(ref)), related() {}
Value::Value(mpz_class mp_integer):
	type(INT),	mp_int(new mpz_class(mp_integer)) {}
Value::Value(const std::vector<std::shared_ptr<Value>>& v):
	type(ARR),	arr(new std::vector<std::shared_ptr<Value>>(v)) {}
Value::Value(const nullptr_t& null):
	type(REF),	ref(new std::shared_ptr<Value>(nullptr)) {}
Value::Value(const Def& def):
	type(DEF),	def(new Def(def)) {}
Value::Value(const Namespace& ns):
	type(NSP),	ns(new Namespace(ns)) {}
Value::Value(const Lambda& lam):
	type(LAM),	lam(new Lambda(lam)) {}
Value::Value(const Object& obj):
	type(OBJ),	obj(new Object(obj)) {}
Value::Value(const std::shared_ptr<Value>& ref, const std::shared_ptr<Value>& related):
	type(REF), ref(new std::shared_ptr<Value>(ref)), related(new std::shared_ptr<Value>(related)) {}

void Value::erase() {
	// dealloc relevant data
	switch (type) {
		case DEC: case EMT:
			return;
		case STR: case MAC:
			delete str; return;
		case INT:
			delete mp_int; return;
		case REF:
			delete related;
		case IMR:
			delete ref;	return;
		case ARR:
			delete arr; return;
		case NSP:
			ns->clear();
			delete ns;
			return;
		case DEF:
			delete def; return;
		case OBJ:
			delete obj; return;
		case LAM:
			delete lam; return;
		default:
			return;
	}
}

Value& Value::set_noerase(const Value& v) {
	type = v.type;

	// switch?
	if (type == DEC) {
		dec = v.dec;
	} else if (type == IMR) {
		ref = new std::shared_ptr<Value>(*v.ref);
	} else if (type == REF) {
		ref = new std::shared_ptr<Value>(*v.ref);
		related = v.related ? new std::shared_ptr<Value>(*v.related) : nullptr;
	} else if (type == STR || type == MAC) {
		str = new std::string(*v.str);
	} else if (type == INT) {
		mp_int = new mpz_class(*v.mp_int);
	} else if (type == ARR) {
		arr = new std::vector<std::shared_ptr<Value>>(*v.arr);
	} else if (type == NSP) {
		ns = new Namespace(*v.ns);
	} else if (type == DEF) {
		def = new Def(*v.def);
	} else if (type == OBJ) {
		obj = new Object(*v.obj);
	} else if (type == LAM) {
		lam = new Lambda(*v.lam);
	}
	return *this;
}

inline static size_t countLines(const std::string& s) {
	size_t lc = 0;
	for (const char c : s)
		if (c == '\n')
			lc++;
	return lc;
}

std::string Value::depict() {
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
		const size_t lines = countLines(*str);
		return "{" + (lines > 1 ? std::to_string(lines) + std::string(" lines"): *str) + " }";
	} else if (type == REF || type == IMR) {
		Value* v = (Value*) defer();
		if (v)
			return v->depict();
		return "cyclic/null reference";
	} else if (type == ARR) {
		std::string ret = "(";
		for (auto& v : *arr)
			ret += v->depict() + ", ";
		ret[ret.length() - 2] = ')';

		return ret.substr(0, ret.length() - 1);
	} else if (type == NSP) {
		std::string ret = "{\n";
		for (const auto& e : *ns)
			ret += "\t\"" + e.first + "\"\t" + Value(e.second).depict() + "\n";
		ret += "} namespace";
		return ret;
	} else if (type == DEF) {
		if (def->native) {
			return "<native> @define";
		} else {
			std::string ret;
			ret += def->_val->depict();
			if (def->run)
				ret += '@';
			ret += "define";
			return ret;
		}
	} else if (type == OBJ) {
		std::string ret = "{";
		for (const auto& v : obj->members)
			ret += "\n\tself." + v.first + " " + v.second->depict();
		if (!obj->members.empty())
			ret += '\n';
		return ret + "} obj";
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
		for (auto v : *arr)
			ret += v->depict() + ", ";
		ret[ret.length() - 1] = ')';
		return ret;
	} else if (type == NSP) {
		std::string ret = "{\n";
		for (const auto& e : *ns)
			ret += "\t\"" + e.first + "\"\t" + Value(e.second).depict() + "\n";
		ret += "} namespace";
		return ret;
	} else if (type == DEF) {
		if (def->native) {
			return "<native> @define";
		} else {
			std::string ret;
			ret += def->_val->depict();
			if (def->run)
				ret += '@';
			ret += "define";
			return ret;
		}
	} else if (type == OBJ) {
		std::string ret = "{";
		for (const auto& v : obj->members)
			ret += "\n\tself." + v.first + " " + v.second->depict();
		if (obj->members.size())
			ret += '\n';
		return ret + "} obj";
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

// get the value that a reference points to
bool Value::deferValue(Value& ret, std::vector<std::shared_ptr<Value>*> pastPtrs) {
	// end of ref recursion
	if (type != REF && type != IMR) {
		ret = *this;
		return true;
	}
	if (!ref || !*ref)
		return false;

	// if it's been seen before it should be cyclic reference
	if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
		return false;

	// follow reference tree
	pastPtrs.emplace_back(ref);
	if (!ref || !*ref)
		return false;

	// trace down refs

	const bool&& found = (*ref)->deferValue(ret, pastPtrs);
	if (found && ret.type == LAM && *related)
		ret.lam->self = *related;
	return found;
}

// get muteable value
// stops at immuteable references
Value* Value::deferMuteable(std::vector<std::shared_ptr<Value>*> pastPtrs) {
	// end of ref recursion
	if (type != REF) // will return on IMR
		return this;
	if (!ref || !*ref)
		return nullptr;

	// if it's been seen before it should be a cyclic reference
	if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
		return nullptr;

	// follow reference tree
	pastPtrs.emplace_back(ref);
	if (!ref || !*ref)
		return nullptr;
	return (*ref)->deferMuteable(pastPtrs);
}

std::shared_ptr<Value> Value::lastRef(std::vector<std::shared_ptr<Value>*> pastPtrs) {
	// end of ref recursion
	if (type != REF && type != IMR)
		return pastPtrs.empty() ? std::make_shared<Value>(*this) : *pastPtrs.back();
	if (!ref || !*ref)
		return nullptr;

	// if it's been seen before it should be cyclic reference
	if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
		return nullptr;

	// follow reference tree
	pastPtrs.emplace_back(ref);
	if (!ref || !*ref)
		return nullptr;

	return (*ref)->lastRef(pastPtrs);
}

const char* Value::typeName() {
	return typeName(type);
}
const char* Value::typeName(const Value::vtype value_type) {
	// could be optimized to an array of strings
	// and using type as index, but im not certain if i can keep the values in correct order
	switch (value_type) {
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
		case NSP: return "namespace";
		case DEF: return "def"; // how did they do this?
		default:
			std::cout <<"unknown type#" <<(const long)value_type <<std::endl;
			return "unknown";
	}
}
bool Value::truthy() {
	if (type == REF) {
		Value* v = (Value*) defer();
		return v && v->truthy();
	}
	if (type == INT)	return *mp_int != 0;
	if (type == DEC)	return dec != 0;
	if (type == EMT)	return false;
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
	// move this to an `is` operator ? prolly nah
	if (type == REF)
		return v.defer() == defer();

	if (type == OBJ)
		return obj->members == v.obj->members;

	// for now at least
	//if (type == LAM || type == OBJ)
	return false;
}