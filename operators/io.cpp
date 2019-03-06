//
// Created by tate on 27-02-19.
//

#include "io.hpp"

namespace op_input {
	const char* name = "input";

	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		std::string inp;
		if (!std::getline(std::cin, inp))
			f.stack.emplace_back(Value());
		else
			f.stack.emplace_back(inp);

		return Frame::Exit();
	}
}

namespace op_print {
	const char* name = "print";

	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to print", f.feed.lineNumber());

		std::cout <<f.stack.back().toString();
		f.stack.pop_back();
		return Frame::Exit();
	}

}

namespace op_println {
	const char* name = "println";

	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to print", f.feed.lineNumber());

		std::cout <<f.stack.back().toString() <<std::endl;
		f.stack.pop_back();

		return Frame::Exit();
	}

}