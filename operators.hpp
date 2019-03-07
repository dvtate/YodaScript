
#ifndef YS2_TOKENS_HPP
#define YS2_TOKENS_HPP

#include <vector>
#include <cstddef>
#include "namespace_def.hpp"
#include "frame.hpp"
#include "code_feed.hpp"


// comment this out if we wanna look professional
#define DEBUG 1

// convert a token namespace to a Token obj
#define OP_NS_TO_TOK(NS) Token({ NS::name, NS::condition, NS::act })

// generate operator declaration
#define OP_NS(NAME)\
namespace NAME {\
	extern const char* name;\
	bool condition(Frame&);\
	Frame::Exit act(Frame&);\
}


#define OP_NS_TO_PAIR(ns) { ns::name, ns::act }


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// this should be included in your error messages so that
//  we where to look if there's a false positive
#ifdef DEBUG
	#define DEBUG_FLI "(" + std::string(__FILENAME__) + "#" + std::to_string(__LINE__) + ")"
#else
	#define DEBUG_FLI ""
#endif

#define DEFER_TOP(FRAME) {\
	const Value* _tmp_v = FRAME.stack.back().defer();\
	FRAME.stack.back().set(_tmp_v ? *_tmp_v : Value(nullptr));\
}

typedef struct Token {

	// if i want to call this from another token
	const char* name;

	// condition for parser
	bool (*condition)(Frame&);

	// run if condition true or if called externally
	Frame::Exit (*act)(Frame&);

} Token;

namespace operators {

	// tokens.find is O(N)
	extern std::vector<Token> tokens;

	// operators.find is O(1)
	extern Namespace operators;

	// operators.find
	Def findOperator(Frame& f);

	// tokens.find
	int findToken(Frame& f);


	bool callByName(Frame& f, const std::string &name, Frame::Exit &exit); // ie - callByName(f, "@", ev)
	bool callOperator(Frame& f, Frame::Exit& exit, const Namespace& ns);
	bool callOperator(Frame& f, Frame::Exit& exit);
	bool callToken(Frame& f, Frame::Exit& exit);
}

#endif //YS2_TOKENS_HPP
