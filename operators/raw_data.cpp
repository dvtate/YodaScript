
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
		frame.stack.push((Value*)nullptr);
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}

}

namespace op_const_true {
	const char* name = "true";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		frame.stack.push(1.0l);
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}
namespace op_const_false {
	const char* name = "false";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		frame.stack.push(0.0l);
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}

namespace op_const_string {
	const char* name = "\"";
	bool condition(Frame& f) {
		return f.feed.tok[0] == '"' || f.feed.tok[0] == '\'';
	}
	Frame::Exit act(Frame& f) {
		const char quoteType = f.feed.body[f.feed.offset];

		std::string s;
		while (f.feed.offset <= f.feed.body.length() || f.feed.getLine()) {

			char c = f.feed.body[++f.feed.offset];
			std::cout <<quoteType << "s(" <<c <<"): " <<s <<std::endl;

			if (c == '\'') {
				if (f.feed.offset >= f.feed.body.length() && !f.feed.getLine())
					return Frame::Exit(Frame::Exit::FEED_END);
				c = f.feed.body[f.feed.offset++];


				if (c == '\\') s += '\\';
				else if (c == 'n') s += '\n';
				else if (c == 'b') s += '\b';
				else if (c == 't') s += '\t';
				else if (c == 'r') s += '\r';
				else if (c == 'f') s += '\f';
				else if (c == 'v') s += '\v';
				else if (c == 'h') {
					const char str[2] = {
							f.feed.body[++f.feed.offset],
							f.feed.body[f.feed.offset + 1]
					};
					char* ptr;
					// convert the hex literal into a char
					s += (char) strtol(str, &ptr, 16);
					f.feed.offset += ptr - str - 1;

				} else if (isdigit(c)) { // octal char
					const char str[3] = {
							f.feed.body[f.feed.offset],
							f.feed.body[f.feed.offset + 1],
							f.feed.body[f.feed.offset + 2],
					};
					char* ptr;
					// convert the octal literal into a char
					s += (char) strtol(str, &ptr, 8);
					f.feed.offset += ptr - str;

				}
			} else if (c == quoteType && f.feed.body[f.feed.offset - 2] != '\\') {
				std::cout <<"DONE";
				f.stack.push(s);
				return Frame::Exit();
			} else {
				s += c;
				//f.feed.offset++;
			}


		}
		return Frame::Exit(Frame::Exit::FEED_END);
	}
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
	//std::cout <<"ignore_until- " <<sstr <<std::endl;
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
	//std::cout <<"ignore_until- " <<chr <<std::endl;
	do {
		for (; feed.offset < feed.body.length(); feed.offset++)
			for (unsigned char c = 0; c < strlen(chr); c++)
				if (chr[c] == feed.body[feed.offset])
					return true;
	} while (feed.getLine());

	return false;
}

bool find_list (CodeFeed& feed, std::string& ret) {
	size_t start = ++feed.offset;

	// starting at indentation level 1
	int indLvl = 1;

	while (indLvl > 0) {

		char c = feed.body[feed.offset++];


		if (c == '(') {
			indLvl++;
		} else if (c == ')') {
			indLvl--;

			// multiline comments
		} else if (c == '/' && feed.body[feed.offset] == '*') {
			feed.offset++;
			if (!ignore_until(feed, "*/")) {
				std::cerr <<"unterminated comment in list\n";
				return false;
			}
			// line comments
		} else if (c == '#') {
			if (!ignore_until_c(feed, "\n")) {
				std::cerr <<"EOF in list\n";
				return false;
			}


			// strings
		} else if (c == '\"') {
			do {
				if (!ignore_until_c(feed, "\"")) {
					std::cerr <<"Unterminated string in list\n";
					return false;
				}
			} while (feed.body[feed.offset - 1] == '\\');


			// also strings?
		} else if (c == '\'') {
			do {
				if (!ignore_until_c(feed, "\'")) {
					std::cerr <<"Unterminated string(') in list\n";
					return false;
				}
			} while (feed.body[feed.offset - 1] == '\\');

			// list
		} else if (c == '{') {
			std::string s;
			if (!find_list(feed, s)) {
				std::cerr <<"unterminated macro within list\n";
				return false;
			}
		}
	}



	feed.offset -= 3;

	// push trimmed macro onto the stack
	ret = feed.body.substr(start, feed.offset);


	feed.offset += 3;
	return true;


}

bool find_macro (CodeFeed& feed, std::string& ret) {
	size_t start = ++feed.offset;

	// starting at indentation level 1
	int indLvl = 1;

	while (indLvl > 0) {

		if (feed.offset >= feed.body.length() && !feed.getLine())
				return false;

		char c = feed.body[feed.offset++];

		if (c == '{') {
			indLvl++;
		} else if (c == '}') {
			indLvl--;

			// multiline comments
		} else if (c == '/' && feed.body[feed.offset] == '*') {
			feed.offset++;
			if (!ignore_until(feed, "*/")) {
				std::cerr <<"unterminated comment in macro\n";
				return false;
			}
			// line comments
		} else if (c == '#') {
			if (!ignore_until_c(feed, "\n")) {
				std::cerr <<"EOF in macro\n";
				return false;
			}


			// strings
		} else if (c == '\"') {
			do {
				if (!ignore_until_c(feed, "\"")) {
					std::cerr <<"Unterminated string in macro\n";
					return false;
				}
			} while (feed.body[feed.offset - 1] == '\\');


			// also strings?
		} else if (c == '\'') {
			do {
				if (!ignore_until_c(feed, "\'")) {
					std::cerr <<"Unterminated string(') in macro\n";
					return false;
				}
			} while (feed.body[feed.offset - 1] == '\\');

			// list
		} else if (c == '(') {
			std::string s;
			if (!find_list(feed, s)) {
				std::cerr <<"unterminated list within macro\n";
				return false;
			}
		}
	}


	feed.offset -= 3;

	// push trimmed macro onto the stack
	ret = feed.body.substr(start, feed.offset);


	feed.offset += 3;
	return true;

}

// parse macro literals
namespace op_const_macro {
	const char* name = "{";
	bool condition(Frame& frame) {
		return frame.feed.tok[0] == '{';
	}

	Frame::Exit act(Frame& f) {

		std::string mac;
		if (!find_macro(f.feed, mac))
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", "EOF while scanning for brace enclosed macro");

		// push trimmed macro onto the stack
		f.stack.push(Value(Value::MAC, mac));

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
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to print", frame.feed.lineNumber());

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
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", std::string(name) + " expected a value to stringify", frame.feed.lineNumber());
		frame.stack.top() = frame.stack.top().repr();
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}