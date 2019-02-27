

#include "boolean_ops.hpp"

/*
OP_NS(op_equals_to)
OP_NS(op_gt)
OP_NS(op_lt)
OP_NS(op_ge)
OP_NS(op_le)
OP_NS(op_ne)
OP_NS(op_is)
OP_NS(op_not)
OP_NS(op_and)
OP_NS(op_or)
 */
namespace op_equals_to {
	const char* name = "==";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}


}