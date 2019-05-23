
#ifndef YS2_TOKENS_HPP
#define YS2_TOKENS_HPP

#include <vector>
#include <cstddef>
#include "extend.hpp"
#include "frame.hpp"
#include "code_feed.hpp"

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
