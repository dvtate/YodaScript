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
		f.feed.offset += strlen(name);
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

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		if (frame.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print", frame.feed.lineNumber());

		std::cout <<frame.stack.back().toString();
		frame.stack.pop_back();
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}

}

namespace op_println {
	const char* name = "println";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		if (frame.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print", frame.feed.lineNumber());

		std::cout <<frame.stack.back().toString() <<std::endl;
		frame.stack.pop_back();

		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}

}
