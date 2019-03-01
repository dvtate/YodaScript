

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
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare", f.feed.lineNumber());

		bool cmp;
		Value v1 = f.stack.back();
		f.stack.pop_back();
		f.stack.back() = (unsigned long) (v1 == f.stack.back());

		return Frame::Exit();
	}


}