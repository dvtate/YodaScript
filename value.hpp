#ifndef YS2_VALUE_HPP
#define YS2_VALUE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <gmp.h>
#include <gmpxx.h>
#include <algorithm>
#include <memory>


/*
* - could contain any reperesentable value
* - union + enum system to preserve memory
* -
*
*/


class Value {
public:

	// what kind of data is contained
	enum vtype {
		EMT = 0, // empty    - value of nothing
				// no value, equivalent to undefined

		DEC, // float       - double
			// double value

		STR, // string       - collection of chars
			// string

		REF, // reference    - reference to memory address of a Value
			// shared_ptr

		MAC, // macro        - framed block of code
			// string

		INT, // integer
			// gnu multiple precision int

		ARR, // array        - collection of Values
			// vector of values

		OBJ, // object/dict  - fancy dict
		    // will receive dedicated class eventually

		LAM, // lambda       - fancy function
			// will receive dedicated class eventually

		/* could be added in future:
		 CHR ? character		- prolly not bc international == confusion
		 BLN ? boolean			- prolly not, truthy values are fine
		 RXP ? reg exp			- prolly not, could be added through lang extension
		*/

	} type { EMT };

	// actual data contained
	union {
		double dec;
		mpz_class* mp_int;
		std::string* str;

		//Value* ref;

		std::shared_ptr<Value>* ref;
		std::vector<Value>* arr;
		// obj
		// lambda
	};

	Value():
		type(Value::EMT) {}
	Value(const vtype t): type(t) {}
	Value(const vtype t, const std::string v):
			type(t), str(new std::string(v)) {}
	Value(const char* v):
		type(Value::STR), str(new std::string(v)) {}
	Value(const std::string v):
		type(Value::STR), str(new std::string(v)) {}
	Value(const double v):
		type(Value::DEC), dec(v) {}
	Value(std::shared_ptr<Value> ref):
		type(Value::REF), ref(new std::shared_ptr<Value>(ref)) {}
	Value(mpz_class mp_integer):
		type(Value::INT), mp_int(new mpz_class(mp_integer)) {}
	Value(const std::vector<Value>& v):
		type(Value::ARR), arr(new std::vector<Value>(v)) {}
	Value(const nullptr_t& null):
		type(REF), ref(new std::shared_ptr<Value>(nullptr)) {}




	// prevent memory leaks when changing the value
	void erase() {
		// only data on heap needs to be deleted
		if (type < Value::STR)
			return;
		if (type == Value::STR || type == Value::MAC)
			delete str;
		if (type == Value::INT)
			delete mp_int;
		if (type == REF)
			delete ref;
		if (type == Value::ARR)
			delete arr;

	}
	~Value(){
		erase();
	}

		
	// set self to given value
	Value& set(const Value& v) {
		erase();
		return set_noerase(v);
	}

	// set self to given value
	inline Value& set_noerase(const Value& v) {
		type = v.type;

		if (type == DEC) {
			dec = v.dec;
		} else if (type == REF) {
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

	// copy
	Value(const Value& v)
		{ set_noerase(v); std::cout <<"copy\n";}
	Value& operator=(const Value& v)
		{ return set(v); }

	std::string repr();
	std::string toString();

	// get the value that a reference points to
	Value* defer(std::vector<std::shared_ptr<Value>*> pastPtrs = {}) {
		// end of ref recursion
		if (type != REF)
			return this;

		// if it's been seen before it should be cyclic reference
		if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
			return nullptr;

		// follow reference tree
		pastPtrs.emplace_back(ref);
		if (!ref || !*ref)
			return nullptr;
		return (*ref)->defer(pastPtrs);
	}


	const char* typeName() {
		// could be optimized to an array of strings
		// and using type as index, but im not certain if i can keep the values in correct order
		switch (type) {
			case EMT: return "empty";
			case INT: return "int";
			case DEC: return "float";
			case REF: return "reference";
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

	bool truthy() {
		if (type == EMT)	return false;
		if (type == REF)	return ref && *ref != nullptr;
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
	bool operator==(Value& v) {
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
};


#endif //YS2_VALUE_HPP
