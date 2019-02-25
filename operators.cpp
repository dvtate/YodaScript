
#include <iostream>
#include <deque>
#include "operators.hpp"
#include "operators/raw_data.hpp"
#include "operators/ctl_flow.hpp"
#include "operators/stack_ctl.hpp"
#include "operators/types.hpp"


int findOperator(Frame& f)
{


	//std::cout <<"looking for operator...";

	// get first token once so that we dont have to find it for every operator
	// stored in feed.tok
	if (!f.feed.setTok()) {
		f.feed.offset = f.feed.body.length() - 1;
		return -2;
	}

	/*
	std::cout <<"findop:\n";
	std::cout <<"\ttoken: \'" <<f.feed.tok <<"\'\n";
	std::cout <<"\tbody[offset]: \'" <<f.feed.fromOffset() <<"\'\n";
	 */

	for (int i = 0; i < operators.size(); i++)
		if (operators[i].condition(f))
			return i;

	return -1;
}



inline static std::deque<struct Token> genTokens() {
	// loads all operators from operators/
	std::deque<struct Token> ret = {

			// const values
			OP_NS_TO_TOK(op_const_empty),
			OP_NS_TO_TOK(op_const_null),
			OP_NS_TO_TOK(op_const_true),
			OP_NS_TO_TOK(op_const_false),

			// literals
			OP_NS_TO_TOK(op_const_macro),
			OP_NS_TO_TOK(op_const_string),

			// data representation
			OP_NS_TO_TOK(op_println),
			OP_NS_TO_TOK(op_print),

			// type conversion
			OP_NS_TO_TOK(op_str),
			OP_NS_TO_TOK(op_typeof),

			// comments
			OP_NS_TO_TOK(op_line_comment),
			OP_NS_TO_TOK(op_multiline_comment),

			// structures
			OP_NS_TO_TOK(op_repeat_loop),


			// more literals (slower execution time
			OP_NS_TO_TOK(op_const_int),
			OP_NS_TO_TOK(op_const_number),


			OP_NS_TO_TOK(op_stk_clear),
			OP_NS_TO_TOK(op_stk_dup),
			OP_NS_TO_TOK(op_stk_swap),
			OP_NS_TO_TOK(op_stk_pop),
			OP_NS_TO_TOK(op_stk_size),
	};

	return ret;
}

std::deque<struct Token> operators = genTokens();
