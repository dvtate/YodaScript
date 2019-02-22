
#include <cstring>
#include <iostream>
#include "raw_data.hpp"


namespace op_const_empty {

	const char* name = "empty";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		// push an empty value onto the stack
		frame.stack.push(Value());
		std::cout << "pushed `empty` onto stack!";

		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}

namespace op_const_null {
	const char* name = "null";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		// push an empty value onto the stack
		frame.stack.push(Value());
		std::cout << "pushed `null` onto stack!";

		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}

}

namespace op_const_string {

}

namespace op_const_number {
	const char* name = "*num";
	bool condition(Frame& frame) {
		// this is temporary i guess
		// exceptions are kinda slow
		try {
			std::cout<<"is it number?...";
			stold(frame.feed.body.substr(frame.feed.offset, frame.feed.body.length()));
			std::cout<<"its a number\n";
		} catch (std::invalid_argument) {
			return false;
		}
		return true;
	}

	Frame::Exit act(Frame& frame) {
		size_t end;

		std::cout <<"makin number...";
		// push number to stack
		frame.stack.push(stold(
				frame.feed.body.substr(frame.feed.offset, frame.feed.body.length()), &end));

		frame.feed.offset += end;

		std::cout <<"done\n";
		return Frame::Exit();
	}

}

namespace op_print {
	const char* name = "print";

	bool condition(Frame& frame) {
		std::cout <<"\'" <<frame.feed.tok <<"\' vs \'" <<name <<"\' = " <<(frame.feed.tok == name);
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		if (!frame.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print");

		std::cout <<frame.stack.top().toString();
		frame.stack.pop();

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
		if (!frame.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print");

		std::cout <<frame.stack.top().toString();
		frame.stack.pop();

		frame.feed.offset += strlen(name);
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
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print");

		std::cout <<frame.stack.top().repr();
		frame.stack.pop();

		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}