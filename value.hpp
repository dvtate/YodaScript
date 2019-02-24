#ifndef YS2_VALUE_HPP
#define YS2_VALUE_HPP

#include <string>
#include <iostream>

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
		NUM, // number       - double
		REF, // reference    - reference to memory address of a Value
		STR, // string       -
		ARR, // array        -
		MAC, // macro        - framed block
		OBJ, // object/dict  -
		LAM, // lambda       -

		/* could be added in future:
		INT ? integer		- prolly not bc would want arbitrary precision
		CHR ? character		- prolly not bc international stuff
		*/

	} type;

	// actual data contained
	union {
		long double number;
		std::string* str;

		Value* ref;

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
	Value(const long double v):
		type(Value::NUM), number(v) {}
	Value(Value* v):
		type(Value::REF), ref(v) {}


	// prevent memory leaks when changing the value
	void erase()
	{
		// only data on heap needs to be deleted
		if (type < Value::STR)
			return;
		if (type == STR || type == MAC) {
			delete str;
		}

	}
	~Value()
		{ erase(); }

		
	// set self to given value
	Value& set(const Value& v)
	{
		erase();
		type = v.type;

		if (type == EMT)
			ref = nullptr;
		else if (type == NUM)
			number = v.number;
		else if (type == REF)
			ref = v.ref;
		else if (type == STR || type == MAC)
			str = new std::string(*v.str);


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

	Value& defer()
		{ return type == REF ? ref->defer() : *this; }
};


#endif //YS2_VALUE_HPP
