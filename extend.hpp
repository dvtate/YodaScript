//
// Created by tate on 01-03-19.
//

#ifndef YS2_NAMESPACE_HPP
#define YS2_NAMESPACE_HPP

#include <string>
#include <unordered_map>


class Value;
class Frame;
class Exit;

// definition - gets run as soon as it's placed onto stack (in between tokens)
class Def {
public:

	bool native;	// use the fxnptr?
	bool run;
	Value* _val;
	Exit (*act)(Frame&);

	Def(const Value& value, const bool runnable = false);
	Def(Exit (*action)(Frame&));
	Def(const Def& def);
	Def();
	~Def();
};


// collection of labelled defs
typedef std::unordered_map<std::string, Def> Namespace;



// for handling things which aren't simple space delimited operators
// ie- adding a new literal type
typedef struct Token {

	// if i want to call this from another token/operator
	const char* name;

	// condition for parsing
	bool (*condition)(Frame&);

	// run if condition true or if called externally
	Exit (*act)(Frame&);

} Token;



#endif //YS2_NAMESPACE_HPP
