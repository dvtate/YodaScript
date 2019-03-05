#ifndef YS2_VALUE_HPP
#define YS2_VALUE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <gmp.h>
#include <gmpxx.h>
#include <algorithm>
#include <memory>
#include <cstddef>
#include <unordered_map>

typedef decltype(nullptr) nullptr_t;

/*
* - could contain any reperesentable value
* - union + enum system to preserve memory
* -
*
*/

#include "namespace.hpp"
#include "object.hpp"

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

		IMR, // immuteable reference - reference to memory that isn't allowed to be changed (change this object)
			// identical to REF in every other way
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

		DEF, // defined term
		//

		NSP, // namespace

		/* could be added in future:
		 DEF ? definition		- contains struct Def { function pointer || shared_ptr<Macro> } which gets run as soon as it is created
		 NSP ? namespace		- see dvtate/planning/yoda/namespaces
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
		std::vector<std::shared_ptr<Value>>* arr;
		// obj
		// lambda

		std::unordered_map<std::string, Def>* ns;
		Def* def;
	};


	Value();
	Value(const vtype);
	Value(const vtype, const std::string);
	Value(const char*);
	Value(const std::string&);
	Value(const double);
	Value(std::shared_ptr<Value>);
	Value(const vtype t, const std::shared_ptr<Value>&);
	Value(mpz_class);
	Value(const std::vector<std::shared_ptr<Value>>&);
	Value(const nullptr_t&);
	Value(const Def&);
	Value(const std::unordered_map<std::string, Def>&);


	// prevent memory leaks when changing the value
	void erase();
	~Value(){
		erase();
	}

		
	// set self to given value
	Value& set(const Value& v) {
		erase();
		return set_noerase(v);
	}

	// set self to given value
	Value& set_noerase(const Value& v);

	// copy
	Value(const Value& v) {
		set_noerase(v);
		//std::cout <<"copy\n";
	}
	Value& operator=(const Value& v)
		{ set(v); return *this;}

	std::string depict();		// represent value
	std::string toString();	// stringify value

	// get the value that a reference points to
	const Value* defer(std::vector<std::shared_ptr<Value>*> pastPtrs = {});

	// get muteable value
	// stops at immuteable references
	Value* deferMuteable(std::vector<std::shared_ptr<Value>*> pastPtrs = {});

	const char* typeName();
	static const char* typeName(const vtype value_type);
	bool truthy();
	bool operator==(Value& v);
};


#endif //YS2_VALUE_HPP
