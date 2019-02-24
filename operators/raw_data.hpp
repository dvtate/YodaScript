//
// Created by tate on 21-02-19.
//

#ifndef YS2_RAW_DATA_HPP
#define YS2_RAW_DATA_HPP

#include "../operators.hpp"

// const values
OP_NS(op_const_empty);
OP_NS(op_const_null);
OP_NS(op_const_true);
OP_NS(op_const_false);

// literals
OP_NS(op_const_number);
OP_NS(op_const_string);
OP_NS(op_const_macro);
OP_NS(op_const_list);

// data representation
// maybe move somewhere else?
OP_NS(op_print);
OP_NS(op_println);

// type conversion
// maybe somewhere else?
OP_NS(op_str);



#endif //YS2_RAW_DATA_HPP
