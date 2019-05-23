//
// Created by tate on 23-05-19.
//

#ifndef YS2_OPERATOR_PPCS_HPP
#define YS2_OPERATOR_PPCS_HPP

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

#endif //YS2_OPERATOR_PPCS_HPP
