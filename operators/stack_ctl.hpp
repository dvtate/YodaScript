//
// Created by tate on 22-02-19.
//

#ifndef YS2_STACK_CTL_HPP
#define YS2_STACK_CTL_HPP


#include "../operators.hpp"

OP_NS(op_stk_pop);
OP_NS(op_stack);
namespace stack {
	Frame::Exit clear(Frame& f);
	Frame::Exit dup(Frame& f);
	Frame::Exit swap(Frame& f);
	Frame::Exit pop(Frame& f);
	Frame::Exit size(Frame& f);
	Frame::Exit reverse(Frame& f);
	Frame::Exit list(Frame& f);
	extern const Namespace stack_ns;
}

#endif //YS2_STACK_CTL_HPP