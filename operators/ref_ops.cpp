//
// Created by tate on 22-02-19.
//

#include "ref_ops.hpp"

namespace op_var_literal {
	const char* name = "$*";
	bool condition(Frame& f) {
		return f.feed.tok[0] == '$';
	}
	Frame::Exit act(Frame& f) {
		// push corresponding reference onto stack
		f.stack.emplace_back(f.getVar(f.feed.tok.substr(1, f.feed.tok.length() - 1)));
		f.feed.offset += f.feed.tok.length();
		return Frame::Exit();
	}
}

namespace op_var_op {
	const char* name = "$";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (!f.stack.size() || f.stack.back().type != Value::STR)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", "$ operator expected a string variable name", f.feed.lineNumber());

		f.stack.back() = Value(f.getVar(*f.stack.back().str));
		f.feed.offset++;
		return Frame::Exit();
	}
}

namespace op_equals {
	const char* name = "=";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset++;
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", "= expected a reference and a value to assign", f.feed.lineNumber());

		Value v2 = f.stack.back();
		f.stack.pop_back();
		Value v1 = f.stack.back();
		f.stack.pop_back();

		Value* ref;
		Value* v;
		if (v1.type == Value::REF) {
			ref = &v1;
			v = &v2;
		} else if (v1.type == Value::REF) {
			ref = &v2;
			v = &v1;
		} else {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", "= requires a reference to assign", f.feed.lineNumber());
		}

		ref->defer().set(*v);

		return Frame::Exit();
	}
}

namespace op_copy_value {
	const char* name = "~";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset++;
		if (!f.stack.size() || f.stack.back().type != Value::REF)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", "~ expected a reference to defer", f.feed.lineNumber());

		f.stack.back() = f.stack.back().defer();

		return Frame::Exit();
	}
}
OP_NS(op_copy_value);