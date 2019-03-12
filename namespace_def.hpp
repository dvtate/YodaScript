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
	~Def();
};


// collection of labelled defs
typedef std::unordered_map<std::string, Def> Namespace;


#endif //YS2_NAMESPACE_HPP
