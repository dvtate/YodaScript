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

		f.stack.back() = Value(f.stack.back().typeName());
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}
}

namespace op_str {
	const char* name = "str";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		if (!frame.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI+ std::string(name) + " expected a value to stringify", frame.feed.lineNumber());
		frame.stack.back() = frame.stack.back().repr();
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}