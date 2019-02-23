
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



typedef struct Token {

	// if i want to call this from another token
	const char* name;

	// condition for parser
	bool (*condition)(Frame&);

	// run if condition true or if called externally
	Frame::Exit (*act)(Frame&);

} Token;


/* likely will need to have 3 layers
*  0. above user level (ie- literals)
*  1. user level (ie- defs and imports
*  2. below user level (ie- operators)
*/

extern std::deque<Token> operators;
int findOperator(Frame& f);


#endif //YS2_TOKENS_HPP
