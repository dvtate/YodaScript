
#include <cstring>
#include <string>
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
			//std::cout<<"is it number?...";
			stold(frame.feed.body.substr(frame.feed.offset, frame.feed.body.length()));
			//std::cout<<"its a number\n";
		} catch (std::invalid_argument) {
			return false;
		}
		return true;
	}

	Frame::Exit act(Frame& frame) {
		size_t end;

		//std::cout <<"makin number...";
		// push number to stack

		std::string num = frame.feed.body.substr(frame.feed.offset, frame.feed.body.length());
		//std::cout <<"numstr: " <<num <<std::endl;

		frame.stack.push(stold(num, &end));
		//std::cout <<"numstr.end = " <<end;

		frame.feed.offset += end;

		//std::cout <<"done\n";
		return Frame::Exit();
	}

}


inline static bool ignore_until(CodeFeed& feed, const char* sstr) {
search:
	size_t res = feed.body.find(sstr, feed.offset);
	if (res == std::string::npos) {
		if (!feed.getLine())
			return false;
		goto search;
	}
	feed.offset += res;
	return true;

}

inline static bool ignore_until_c(CodeFeed& feed, const char* chr) {

	do {
		for (; feed.offset < feed.body.length(); feed.offset++)
			for (int c = 0; c < strlen(chr); c++)
				if (chr[c] == feed.body[feed.offset])
					return true;
	} while (feed.getLine());

	return false;
}

// parse macro literals
namespace op_const_macro {
	const char* name = "{";
	bool condition(Frame& frame) {
		return frame.feed.tok[0] == '{';
	}

	Frame::Exit act(Frame& f) {
		size_t start = ++f.feed.offset;

		// starting at indentation level 1
		int indLvl = 1;

		while (indLvl > 0) {

			char c = f.feed.body[f.feed.offset++];


			if (c == '{') {
				indLvl++;
			} else if (c == '}') {
				indLvl--;

			// multiline comments
			} else if (c == '/' && f.feed.body[f.feed.offset] == '*') {
				f.feed.offset++;
				if(!ignore_until(f.feed, "*/"))
					break;

			// line comments
			} else if (c == '#') {
				if(!ignore_until_c(f.feed, "\n"))
					break;

			// strings
			} else if (c == '\"') {
				if(!ignore_until_c(f.feed, "\""))
					break;

			// also strings?
			} else if (c == '\'') {
				if (!ignore_until_c(f.feed, "\'"))
					break;
			}
		}


		if (indLvl > 0)
			return Frame::Exit(Frame::Exit::FEED_END, "EOF while scanning for brace enclosed macro");

		f.feed.offset -= 3;

		// push trimmed macro onto the stack
		f.stack.push(Value(Value::MAC, f.feed.body.substr(start, f.feed.offset)));


		f.feed.offset += 3;

		// std::cout <<"Macro received: {\n" <<f.feed.body.substr(start, f.feed.offset) <<"\n}";

		return Frame::Exit();
	}
}


namespace op_print {
	const char* name = "print";

	bool condition(Frame& frame) {
		//std::cout <<"\'" <<frame.feed.tok <<"\' vs \'" <<name <<"\' = " <<(frame.feed.tok == name);
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

		std::cout <<frame.stack.top().toString() <<std::endl;
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
		frame.stack.top() = frame.stack.top().repr();
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}