

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
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare", f.feed.lineNumber());

		Value v1 = f.stack.back();
		f.stack.pop_back();
		f.stack.back() = (unsigned long) (! (v1 == f.stack.back()));

		return Frame::Exit();
	}
}

// TODO: make support Objects via overloading
namespace op_gt {
	const char* name = ">";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare",
							   f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		DEFER_TOP(f);
		auto TypeError = [&](const Value::vtype t1, const Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   std::string(name) + ": invalid argument types: " + Value::typeName(t1) + " & " +
							   Value::typeName(t2) + "\n", f.feed.lineNumber());
		};

		switch (v2.type) {
			case Value::STR:
				if (f.stack.back().type != Value::STR)
					return TypeError(f.stack.back().type, v2.type);
				f.stack.back().set(mpz_class(*f.stack.back().str > *v2.str));
				break;
			case Value::INT:
				if (f.stack.back().type == Value::INT)
					f.stack.back().set(*f.stack.back().mp_int > *v2.mp_int);
				else if (f.stack.back().type == Value::DEC)
					f.stack.back().set(f.stack.back().dec > *v2.mp_int);
				else
					return TypeError(f.stack.back().type, v2.type);
				break;
			case Value::DEC:
				if (f.stack.back().type == Value::DEC)
					f.stack.back().set(f.stack.back().dec > v2.dec);
				else if (f.stack.back().type == Value::INT)
					f.stack.back().set(*f.stack.back().mp_int > v2.dec);
				else
					return TypeError(f.stack.back().type, v2.type);
				break;
			default:
				return TypeError(f.stack.back().type, v2.type);
		}

		return Frame::Exit();
	}
}


namespace op_lt {
	const char *name = "<";

	bool condition(Frame &f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame &f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected 2 values to compare",
							   f.feed.lineNumber());

		DEFER_TOP(f);
		Value v2 = f.stack.back();
		f.stack.pop_back();

		DEFER_TOP(f);
		auto TypeError = [&](const Value::vtype t1, const Value::vtype t2) {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError",
							   std::string(name) + ": invalid argument types: " + Value::typeName(t1) + " & " +
							   Value::typeName(t2) + "\n", f.feed.lineNumber());
		};

		switch (f.stack.back().type) {
			case Value::STR:
				switch (v2.type) {
					case Value::STR:
						f.stack.back().set(*f.stack.back().str < *v2.str);
						break;
					default:
						return TypeError(f.stack.back().type, v2.type);
				}
				break;
			case Value::INT:
				switch (v2.type) {
					case Value::INT:
						f.stack.back().set(*f.stack.back().mp_int < *v2.mp_int);
						break;
					case Value::DEC:
						f.stack.back().set(f.stack.back().mp_int->get_d() < v2.dec);
						break;
					default:
						return TypeError(f.stack.back().type, v2.type);
				}
				break;
			case Value::DEC:
				switch (v2.type) {
					case Value::DEC:
						f.stack.back().set(f.stack.back().dec < v2.dec);
						break;
					case Value::INT:
						f.stack.back().set(f.stack.back().dec < v2.mp_int->get_d());
						break;
					default:
						return TypeError(f.stack.back().type, v2.type);
				}

			default:
				return TypeError(f.stack.back().type, v2.type);

		}

		return Frame::Exit();
	}
}


namespace op_and {
	const char* name = "&&";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "&& expected to conditionals");

		// technically short-circuit logic is in reverse order but expressions already get evaluated anyway
		bool v1 = f.stack.back().truthy();
		f.stack.pop_back();
		f.stack.back().set(v1 && f.stack.back().truthy());

		return Frame::Exit();
	}
}

namespace op_or {
	const char* name = "||";
	bool condition(Frame& f)
		{ return f.feed.tok == name; }

	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "|| expected 2 conditionals", f.feed.lineNumber());

		// technically short-circuit logic is in reverse order but expressions already get evaluated anyway
		bool v1 = f.stack.back().truthy();
		f.stack.pop_back();
		f.stack.back().set(v1 || f.stack.back().truthy());

		return Frame::Exit();
	}
}