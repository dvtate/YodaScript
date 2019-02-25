#ifndef YS2_VALUE_HPP
#define YS2_VALUE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <gmp.h>
#include <gmpxx.h>
#include <algorithm>

/*
*
*
*
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

		REF, // reference    - reference to memory address of a Value
			// Value* pointer

		STR, // string       - collection of chars
			// string

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

		Value* ref;

		std::vector<Value>* arr;
		// obj
		// lambda
	};


	Value():
		type(Value::EMT), ref(nullptr) {}
	Value(const vtype t): type(t) {}
	Value(const vtype t, const std::string v):
			type(t), str(new std::string(v)) {}
	Value(const char* v):
		type(Value::STR), str(new std::string(v)) {}
	Value(const std::string v):
		type(Value::STR), str(new std::string(v)) {}
	Value(const double v):
		type(Value::DEC), dec(v) {}
	Value(const Value* v):
		type(Value::REF), ref((Value*) v) {}
	Value(mpz_class mp_integer):
		type(Value::INT), mp_int(new mpz_class(mp_integer)) {}






	// prevent memory leaks when changing the value
	void erase() {
		// only data on heap needs to be deleted
		if (type < Value::STR)
			return;
		if (type == Value::STR || type == Value::MAC)
			delete str;
		if (type == Value::INT)
			delete mp_int;
		if (type == Value::ARR)
			delete arr;

	}
	~Value()
		{ erase(); }

		
	// set self to given value
	Value& set(const Value& v) {
		erase();
		type = v.type;

		if (type == EMT)
			ref = nullptr;
		else if (type == DEC)
			dec = v.dec;
		else if (type == REF)
			ref = v.ref;
		else if (type == STR || type == MAC)
			str = new std::string(*v.str);
		else if (type == INT)
			mp_int = new mpz_class(*v.mp_int);

		return *this;
	}

	// copy
	Value(const Value& v)
		{ set(v); }
	Value& operator=(Value v)
		{ return set(v); }

	bool isNull() {
		return type == Value::REF && !ref;
	}


	std::string repr();
	std::string toString();

	Value* defer(std::vector<Value*> pastPtrs = {}) {
		if (type != REF)
			return this;

		if (std::find(pastPtrs.begin(), pastPtrs.end(), ref) != pastPtrs.end())
			return nullptr;
		pastPtrs.emplace_back(ref);
		return defer(pastPtrs);
	}
	const char* typeName() {

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
				std::cout <<"type#" <<type <<std::endl;
				return "unknown";
		}
	}
};


#endif //YS2_VALUE_HPP
