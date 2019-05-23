//
// Created by tate on 22-02-19.
//

#ifndef YS2_REF_OPS_HPP
#define YS2_REF_OPS_HPP


#include "../operator_ppcs.hpp"

OP_NS(op_var_literal);
OP_NS(op_var_op);

OP_NS(op_equals);
OP_NS(op_set);
OP_NS(op_let);

// weak/strong refs
OP_NS(op_const);
OP_NS(op_strong);

// defer value
OP_NS(op_copy_value);

// debug
OP_NS(op_vars);
OP_NS(op_trace_ref);

OP_NS(op_ref);
OP_NS(op_var_name);



#endif //YS2_REF_OPS_HPP
