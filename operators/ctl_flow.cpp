//
// Created by tate on 22-02-19.
//


#include <cmath>
#include "ctl_flow.hpp"


// # comments
namespace op_line_comment {
	const char* name = "#";
	bool condition(Frame& f) {
		return f.feed.body.at(f.feed.offset) == '#';
	}
	Frame::Exit act(Frame& f) {
		while (f.feed.offset < f.feed.body.length() && f.feed.body.at(f.feed.offset) != '\n' )
			f.feed.offset++;

		return Frame::Exit();
	}
}

/* */
namespace op_multiline_comment {
	const char* name = "/*";
	bool condition(Frame& f) {
		return f.feed.body.length() - f.feed.offset >= 2
			&& f.feed.body.at(f.feed.offset) == '/'
			&& f.feed.body.at(f.feed.offset + 1) == '*';
	}

	Frame::Exit act(Frame& f) {
		size_t start = f.feed.offset;
		f.feed.offset += strlen(name);

	find_ending:
		//std::cout <<"searching for end\n";

		size_t end = f.feed.body.find("*/", f.feed.offset);
		if (end == std::string::npos) {
			if (!f.feed.getLine())
				return Frame::Exit(Frame::Exit::ERROR, "SyntaxError",
								   "Unterminated multi-line comment", start);
			goto find_ending;
		}

		f.feed.offset += end + 1;

		return Frame::Exit();
	}



}


namespace op_repeat_loop {
	const char* name = "repeat";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);

		const Frame::Exit bad_exit = Frame::Exit(Frame::Exit::ERROR,
				"ArgError", std::string(name) + " expected a macro and a number of times to run it",
				f.feed.lineNumber());

		if (f.stack.size() < 2)
			return bad_exit;


		mpz_class times;
		if (f.stack.back().type == Value::INT)
			times = *f.stack.back().mp_int;
		else if (f.stack.back().type == Value::DEC)
			times = f.stack.back().dec;
		else
			return bad_exit;

		f.stack.pop_back();

		if (f.stack.back().type != Value::MAC)
			return bad_exit;

		const std::string body = *f.stack.back().str;
		f.stack.pop_back();
		Frame loop = f.scope(CodeFeed(body));

		for (uint64_t i = 0; i < times; i++) {
			loop.feed.offset = 0; // feed bodys are now immutable :)
			const Frame::Exit ev = loop.run();
			if (ev.reason == Frame::Exit::ERROR)
				return Frame::Exit(Frame::Exit::ERROR, "In Repeat Loop", "", f.feed.lineNumber(), ev);
		}

		// merge stacks
		f.stack.insert(f.stack.end(), loop.stack.begin(), loop.stack.end());

		return Frame::Exit();

	}
}