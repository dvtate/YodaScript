
#ifndef YS2_TOKENS_HPP
#define YS2_TOKENS_HPP


#include <deque>

#include "frame.hpp"
#include "code_feed.hpp"

// convert a token namespace to a Token obj
#define OP_NS_TO_TOK(NS) Token({ NS::name, NS::condition, NS::act })

// generate operator declaration
#define OP_NS(NAME)\
namespace NAME {\
	extern const char* name;\
	bool condition(Frame&);\
	Frame::Exit act(Frame&);\
}


// this should be included in your error messages so that
//  we where to look if there's a false positive
#ifdef DEBUG
	#define DEBUG_FLI "(" __FILENAME__ "#" __LINE__ ")"
#else
	#define DEBUG_FLI ""
#endif

typedef struct Token {

	// if i want to call this from another token
	const char* name;

	// condition for parser
	bool (*condition)(Frame&);

	// run if condition true or if called externally
	Frame::Exit (*act)(Frame&);

} Token;


extern std::deque<Token> operators;
int findOperator(Frame& f);


#endif //YS2_TOKENS_HPP
