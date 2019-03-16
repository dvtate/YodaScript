//
// Created by tate on 24-02-19.
//


#include "types.hpp"


namespace op_typeof {
	const char* name = "typeof";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (!f.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + "expected a value to find type of", f.feed.lineNumber());

		f.stack.back().set(f.stack.back().typeName());
		return Frame::Exit();
	}
}

namespace op_str {
	const char* name = "str";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		if (frame.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to stringify", frame.feed.lineNumber());
		frame.stack.back().set(frame.stack.back().toString());
		return Frame::Exit();
	}
}

namespace op_depict {
	const char* name = "depict";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to depict", f.feed.lineNumber());
		f.stack.back().set(f.stack.back().depict());
		return Frame::Exit();
	}
}

namespace op_int {
	const char* name = "int";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to convert to an int", f.feed.lineNumber());

		DEFER_TOP(f);
		if (f.stack.back().type == Value::STR) {
			try {
				f.stack.back().set(mpz_class(*f.stack.back().str));
			} catch (const std::invalid_argument&) {
				return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " int received an invalid string", f.feed.lineNumber());
			}
		} else if (f.stack.back().type == Value::INT) {
			return Frame::Exit();
		} else if (f.stack.back().type == Value::DEC) {
			try {
				f.stack.back().set(mpz_class(f.stack.back().dec));
			} catch (const std::invalid_argument&) {
				return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " int received an invalid float", f.feed.lineNumber());
			}
		} else
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " invalid " + std::string(f.stack.back().typeName()) + " passed to int" );

		return Frame::Exit();
	}
}