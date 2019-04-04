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

#include "extend.hpp"
#include "object.hpp"
#include "lambda.hpp"

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

		DEF, // defined term - gets evaluated as soon as it's put onto stack
			// def ptr

		NSP, // namespace collection of labeled def's
			// typedef in namespace_def.hpp

		/* could be added in future:
		 DRF ? double ref - reference with a related value to prevent it from getting gc'd

		 CHR ? character		- prolly not bc international == confusion
		 BLN ? boolean			- prolly not, truthy values are fine
		 RXP ? reg exp			- prolly not, could be added through lang extension
		*/

	} type { EMT };

	// actual data contained here
	union {
		double dec;
		mpz_class* mp_int;
		std::string* str;

		std::vector<std::shared_ptr<Value>>* arr;

		Object* obj;
		Lambda* lam;

		Namespace* ns;
		Def* def;

		struct {
			std::shared_ptr<Value>* ref;
			std::shared_ptr<Value>* related;
		};
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
	Value(const Namespace&);
	Value(const Lambda& lam);
	Value(const Object& obj);

	// double ref
	Value(const std::shared_ptr<Value>& ref, const std::shared_ptr<Value>& related);

	// prevent memory leaks when changing the value
	// TODO: add object destructor
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

	std::string depict();	// represent value
	std::string toString();	// stringify value

	// gets a value from a ref, if it's a lambda then it adds an if
	bool deferValue(Value& ret, std::vector<std::shared_ptr<Value>*>& pastPtrs);
	bool deferValue(Value& ret) {
		std::vector<std::shared_ptr<Value>*> ptrs; // recycle vector to save memory
		return deferValue(ret, ptrs);
	}

	// get the value that a reference points to
	const Value* defer(std::vector<std::shared_ptr<Value>*>& pastPtrs);
	const Value* defer(bool& imr, std::vector<std::shared_ptr<Value>*>& pastPtrs);
	const Value* defer(){
		std::vector<std::shared_ptr<Value>*> ptrs;
		return defer(ptrs);
	}
	const Value* defer(bool& imr) {
		std::vector<std::shared_ptr<Value>*> ptrs;
		return defer(imr, ptrs);
	}
	// get muteable value
	// stops at immuteable references
	Value* deferMuteable(std::vector<std::shared_ptr<Value>*>& pastPtrs);
	Value* deferMuteable() { // this is to make it so we can use references to prevent copies
		std::vector<std::shared_ptr<Value>*> ptrs;
		return deferMuteable(ptrs);
	}

	Value* deferChange();

	//
	std::shared_ptr<Value> lastRef(std::vector<std::shared_ptr<Value>*>& pastPtrs);
	std::shared_ptr<Value> lastRef() {
		std::vector<std::shared_ptr<Value>*> ptrs;
		return lastRef(ptrs);
	}

	const char* typeName();
	static const char* typeName(vtype value_type);
	bool truthy();
	bool operator==(Value& v);
};


#endif //YS2_VALUE_HPP
