
#include <cstring>
#include <string>
#include <iostream>
#include <gmpxx.h>
#include "raw_data.hpp"


namespace op_const_empty {

	const char* name = "empty";

	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& frame) {
		// push an empty value onto the stack
		frame.stack.emplace_back(Value());
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
		frame.stack.emplace_back((std::shared_ptr<Value>)nullptr);
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
		frame.stack.emplace_back(1.0);
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
		frame.stack.emplace_back(0.0);
		frame.feed.offset += strlen(name);
		return Frame::Exit();
	}
}

namespace op_const_string {
	const char* name = "\"";
	bool condition(Frame& f) {
		return f.feed.tok[0] == '"' || f.feed.tok[0] == '\'';
	}

	// rewrite this pls
	Frame::Exit act(Frame& f) {
		size_t start = f.feed.offset;
		const char quoteType = f.feed.body[f.feed.offset++];

		std::string s;
		while (f.feed.offset < f.feed.body.length() || f.feed.getLine()) {

			char c = f.feed.body.at(f.feed.offset++);
			//std::cout <<quoteType << "s(" <<c <<"): " <<s <<std::endl;

			if (c == '\\') {
				if (f.feed.offset >= f.feed.body.length() && !f.feed.getLine())
					break;
				c = f.feed.body[f.feed.offset++];


				if (c == '\\') s += '\\';
				else if (c == 'n') s += '\n';
				else if (c == 'b') s += '\b';
				else if (c == 't') s += '\t';
				else if (c == 'r') s += '\r';
				else if (c == 'f') s += '\f';
				else if (c == 'v') s += '\v';
				else if (c == 'h') {
					if (f.feed.offset + 2 >= f.feed.body.length() && !f.feed.getLine())
						break;
					const char str[2] = {
							f.feed.body.at(++f.feed.offset),
							f.feed.body.at(f.feed.offset + 1)
					};
					char* ptr;
					// convert the hex literal into a char
					s += (char) strtol(str, &ptr, 16);
					f.feed.offset += ptr - str - 1;

				} else if (isdigit(c)) { // octal char
					if (f.feed.offset + 2 >= f.feed.body.length() && !f.feed.getLine())
						break;
					const char str[3] = {
							f.feed.body.at(f.feed.offset),
							f.feed.body.at(f.feed.offset + 1),
							f.feed.body.at(f.feed.offset + 2),
					};
					char* ptr;
					// convert the octal literal into a char
					s += (char) strtol(str, &ptr, 8);
					f.feed.offset += ptr - str;

				}

			} else if (c == quoteType && f.feed.body[f.feed.offset - 2] != '\\') {
				f.feed.offset++;
				f.stack.emplace_back(s);
				return Frame::Exit();
			} else {
				s += c;
			}

		}
		return Frame::Exit(Frame::Exit::ERROR, "Syntax Error", "Unterminated string literal", f.feed.lineNumber(start));
	}
}

namespace op_const_int {
	const char* name = "*int";
	bool condition(Frame& f) {
		try {
			mpz_class(f.feed.tok);
		} catch (const std::invalid_argument&) {
			return false;
		}
		return true;
	}

	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(mpz_class(f.feed.tok));
		f.feed.offset += f.feed.tok.length();
		return Frame::Exit();
	}
}

namespace op_const_number {
	const char* name = "*num";
	bool condition(Frame& frame) {
		// this is temporary i guess
		// exceptions are kinda slow
		try {
			stold(frame.feed.tok);
		} catch (const std::invalid_argument&) {
			return false;
		}
		return true;
	}

	Frame::Exit act(Frame& frame) {
		size_t end;
		frame.stack.emplace_back(stold(frame.feed.tok, &end));
		frame.feed.offset += end;
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
					std::cerr <<"Unterminated string(\") in list\n";
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

		}
	}

	feed.offset -= 3;

	// send trimmed list inside
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
			feed.offset++;


			// also strings?
		} else if (c == '\'') {
			do {
				if (!ignore_until_c(feed, "\'")) {
					std::cerr <<"Unterminated string(') in macro\n";
					return false;
				}
			} while (feed.body[feed.offset - 1] == '\\');
			feed.offset++;

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

		//std::cout <<mac;
		// push trimmed macro onto the stack
		f.stack.emplace_back(Value(Value::MAC, mac));

		// std::cout <<"Macro received: {\n" <<f.feed.body.substr(start, f.feed.offset) <<"\n}";

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
