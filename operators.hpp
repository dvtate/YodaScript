
#ifndef YS2_TOKENS_HPP
#define YS2_TOKENS_HPP

#include <vector>
#include <cstddef>
#include "extend.hpp"
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

namespace operators {

	// operators.find is O(1)
	extern Namespace operators;

	// tokens.find is O(N)
	extern std::vector<Token> tokens;

	bool callByName(Frame& f, const std::string &name, Frame::Exit &exit); // ie - callByName(f, "@", ev)
	bool callOperator(Frame& f, Frame::Exit& exit, const Namespace& ns);
	bool callOperator(Frame& f, Frame::Exit& exit);
	bool callToken(Frame& f, Frame::Exit& exit);
}

#endif //YS2_TOKENS_HPP
