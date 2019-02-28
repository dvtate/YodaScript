
#include <iostream>
#include <deque>
#include "operators.hpp"
#include "operators/raw_data.hpp"
#include "operators/ctl_flow.hpp"
#include "operators/stack_ctl.hpp"
#include "operators/types.hpp"
#include "operators/ref_ops.hpp"
#include "operators/io.hpp"
#include "operators/boolean_ops.hpp"

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

	// check condition for each operator
	for (int i = 0; i < operators.size(); i++) // O(N) oof
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
			OP_NS_TO_TOK(op_const_list),
			OP_NS_TO_TOK(op_const_string),

			// cio
			OP_NS_TO_TOK(op_println),
			OP_NS_TO_TOK(op_print),
			OP_NS_TO_TOK(op_input),

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

			OP_NS_TO_TOK(op_var_literal),
			OP_NS_TO_TOK(op_var_op),
			OP_NS_TO_TOK(op_equals), // change the value of a variable
			OP_NS_TO_TOK(op_set), 	 // give a variable a new value
			OP_NS_TO_TOK(op_copy_value),
			OP_NS_TO_TOK(op_vars),

			OP_NS_TO_TOK(op_equals_to)
	};

	return ret;
}

std::deque<struct Token> operators = genTokens();
