

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

namespace op_not {
	const char* name = "!";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset++;
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", " ! operator requires a condition to negate.", f.feed.lineNumber());

		f.stack.back().set(f.stack.back().truthy());
		return Frame::Exit();
	}
}
namespace op_equals_to {
	const char* name = "==";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare", f.feed.lineNumber());

		Value v1 = f.stack.back();
		f.stack.pop_back();
		f.stack.back() = (unsigned long) (v1 == f.stack.back());

		return Frame::Exit();
	}
}
namespace op_ne {
	const char* name = "!=";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare", f.feed.lineNumber());

		Value v1 = f.stack.back();
		f.stack.pop_back();
		f.stack.back() = (unsigned long) (! (v1 == f.stack.back()));

		return Frame::Exit();
	}
}

// TODO: make support Objects via overloading
static inline bool cmpType(const Value::vtype& v) {
	return v == Value::DEC || v == Value::INT || v == Value::STR;
}
namespace op_gt {
	const char* name = ">";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v1 = f.stack.back();
		f.stack.pop_back();

		DEFER_TOP(f);
		auto TypeError = [&](const Value::vtype t1, const Value::vtype t2){
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
					std::string(name) + ": invalid argument types: " + Value::typeName(t1) + " & " + Value::typeName(t2) + "\n", f.feed.lineNumber());
		};


		switch (v1.type) {
			case Value::STR:
				if (f.stack.back().type != Value::STR)
					return TypeError(f.stack.back().type, v1.type);
				f.stack.back().set(mpz_class(*f.stack.back().str > *v1.str));
				break;
			case Value::INT:
				if (f.stack.back().type == Value::INT)
					f.stack.back().set(*f.stack.back().mp_int > *v1.mp_int);
				else if (f.stack.back().type == Value::DEC)
					f.stack.back().set(f.stack.back().dec > *v1.mp_int);
				else
					return TypeError(f.stack.back().type, v1.type);
				break;
			case Value::DEC:
				if (f.stack.back().type == Value::DEC)
					f.stack.back().set(f.stack.back().dec > v1.dec);
				else if (f.stack.back().type == Value::INT)
					f.stack.back().set(*f.stack.back().mp_int > v1.dec);
				else
					return TypeError(f.stack.back().type, v1.type);
				break;
			default:
				return TypeError(f.stack.back().type, v1.type);
		}

		return Frame::Exit();
	}
}