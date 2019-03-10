//
// Created by tate on 09-03-19.
//

#include "generic_operators.hpp"


namespace op_size {
	const char* name = "size";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " size expected a sized value", f.feed.lineNumber());

		const Value* v = f.stack.back().defer();
		switch (v->type) {
			case Value::ARR:
				f.stack.back().set(mpz_class(f.stack.back().arr->size()));
				break;
			case Value::STR: case Value::MAC:
				f.stack.back().set(mpz_class(f.stack.back().str->length()));
				break;

			default:
				return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " size expected a sized value", f.feed.lineNumber());
		}

		return Frame::Exit();
	}
}