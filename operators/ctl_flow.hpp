//
// Created by tate on 21-02-19.
//

#ifndef YS2_CTL_FLOW_HPP
#define YS2_CTL_FLOW_HPP

#include "../operator_ppcs.hpp"

OP_NS(op_line_comment)
OP_NS(op_multiline_comment)
OP_NS(op_repeat_loop)
OP_NS(op_exec)
OP_NS(op_eval)
OP_NS(op_cond)
OP_NS(op_while)

OP_NS(op_return)
OP_NS(op_escape)
OP_NS(op_up)

Frame::Exit runMacro(Frame& f, const std::string& macro, const bool merge_stack);

#endif //YS2_CTL_FLOW_HPP
