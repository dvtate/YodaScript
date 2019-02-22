
#include <iostream>
#include <deque>
#include "operators.hpp"
#include "operators/raw_data.hpp"
#include "operators/ctl_flow.hpp"


int findOperator(Frame f)
{
	std::cout <<"looking for operator...";

	// get first token once so that we dont have to find it for every operator
	// stored in feed.tok
	if (!f.feed.setTok()) {
		f.feed.offset = f.feed.body.length() - 1;
		return -1;
	}


	for (int i = 0; i < operators.size(); i++)
		if (operators[i].condition(f))
			return i;

	return -1;
}



inline static std::deque<struct Token> genTokens() {
	// loads all operators from operators/
	std::deque<struct Token> ret = {
		OP_NS_TO_TOK(op_const_empty),
		OP_NS_TO_TOK(op_const_number),
		OP_NS_TO_TOK(op_println),
		OP_NS_TO_TOK(op_print),
		OP_NS_TO_TOK(op_str),
	};

	return ret;
}

std::deque<struct Token> operators = genTokens();
