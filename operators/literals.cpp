
#include <cstring>
#include <string>
#include <iostream>
#include <gmpxx.h>
#include "literals.hpp"

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
				//f.feed.offset++;
				f.stack.emplace_back(s);
				return Frame::Exit();
			} else {
				s += c;
			}

		}
		return Frame::Exit(Frame::Exit::ERROR, "Syntax Error", DEBUG_FLI "Unterminated string literal", f.feed.lineNumber(start));
	}
}

namespace op_const_int {
	const char* name = "*int";
	bool condition(Frame& f) {
		// maybe could look at their source code to optimize this...
		// feels like shit useing try-catch :/
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
	const char* name = "*dec";
	bool condition(Frame& frame) {
		// this is temporary i guess
		// exceptions are kinda slow
		try {
			std::stod(frame.feed.tok);
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
		if (feed.offset >= feed.body.length() && !feed.getLine())
			return false;

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

	// push trimmed macro onto the stack
	ret = feed.body.substr(start, feed.offset - start - 1);

	return true;

}

bool find_macro (CodeFeed& feed, std::string& ret) {
	size_t start = feed.offset;

	// starting at indentation level 1
	int indLvl = 1;

	while (indLvl > 0) {

		if (feed.offset >= feed.body.length() && !feed.getLine())
				return false;

		char c = feed.body[feed.offset++];
		//std::cout <<"c = " <<c <<std::endl;
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

	// push trimmed macro onto the stack
	ret = feed.body.substr(start, feed.offset - start - 1);


	return true;

}

// parse macro literals
namespace op_const_macro {
	const char* name = "{";
	bool condition(Frame& frame) {
		return frame.feed.tok[0] == '{';
	}

	Frame::Exit act(Frame& f) {
		f.feed.offset++;
		std::string mac;
		if (!find_macro(f.feed, mac))
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI "EOF while scanning for brace enclosed macro");

		//std::cout <<mac;
		// push trimmed macro onto the stack
		f.stack.emplace_back(Value(Value::MAC, mac));

		// std::cout <<"Macro received: {\n" <<f.feed.body.substr(start, f.feed.offset) <<"\n}";

		return Frame::Exit();
	}
}

namespace op_const_list {

	// separate different parts of the list into sections
	inline std::vector<std::string> split_list(std::string& str) {

		size_t pos = 0, past = 0;
		uint16_t ldepth = 0; // up to 65k dimensions
		uint16_t sdepth = 0; // up to 65k layers of nested structures
		bool quoted = false; // are we in a string?
		bool line_comment = false; //commented until newline?
		std::vector<std::string> ret;

		for (size_t i = 0; i < str.size(); i++) {

			switch (str[i]) {
				case '(':
					if (!line_comment && !quoted)
						ldepth++;
					break;
				case ')':
					if (!line_comment && !quoted)
						ldepth--;
					break;
				case '{':
					if (!line_comment && !quoted)
						sdepth++;
					break;
				case '}':
					if (!line_comment && !quoted)
						sdepth--;
					break;
				case '\"':
					if (!line_comment)
						if (!(quoted && str[i-1] == '\\')) // make sure quote isn't escaped
							quoted = !quoted;
					break;
				case '#':
					if (!quoted) line_comment = true;
					break;
				case '/': // ignore multi-line comment
					if (str[i + 1] == '*')
						while (i < str.length() - 1
								&& str[i] != '*'
								&& str[++i] != '/');

					break;
				case '\n':
					line_comment = quoted = false;
					break;
				case ',':
					if ( !quoted && !line_comment && ldepth <= 0  && sdepth <= 0) {
						ret.emplace_back(str.substr(past, pos - past));
						past = pos + 1;
					}
					break;
			}
			pos++;
		}

		// gets value after last comma
		ret.push_back(str.substr(past, pos - past));


		return ret;


	}

	const char* name = "(";
	bool condition(Frame& f) {
		return f.feed.tok[0] == '(';
	}
	Frame::Exit act(Frame& f) {
		// TODO: change this to run in same scope as f
		// TODO: fix zero length list literal

		std::vector<std::shared_ptr<Value>> ret;
		std::string l_body;
		if (!find_list(f.feed, l_body))
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI "EOF while scanning for list");

		std::vector<std::string> elems = split_list(l_body);
		//std::shared_ptr<Frame> elem_proc = f.scope(CodeFeed(), false);
		std::vector<Value> proc_stack;
		CodeFeed fcf = f.feed;
		std::swap(f.stack, proc_stack);

		ret.reserve(elems.size());
		for (size_t i = 0; i < elems.size(); i++) {
			f.feed.body = elems[i];
			f.feed.offset = 0;
			Frame::Exit ev = f.run(f.self_ref);
			if (ev.reason == Frame::Exit::ERROR) {
				f.feed = fcf;
				f.stack = proc_stack;
				return Frame::Exit(Frame::Exit::ERROR, "Syntax Error",
								   DEBUG_FLI "Error while processing elem " + std::to_string(i) + " in list literal.",
								   f.feed.lineNumber(), ev);
			}

			ret.emplace_back(std::make_shared<Value>(f.stack.empty() ? Value() : f.stack.back()));
			f.stack.clear();
		}

		f.feed = fcf;
		f.stack = proc_stack;

		// if intended to be an empty list, literal must be `()` or `( )` /\(\s+\)/
		if (ret.size() == 1 && ret[0]->type == Value::EMT && elems[0].length() < 2)
			ret.pop_back();

		f.stack.emplace_back(std::move(ret));

		return Frame::Exit();
	}
}
