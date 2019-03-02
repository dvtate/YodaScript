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
								   DEBUG_FLI "Unterminated multi-line comment", start);
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
				"ArgError", DEBUG_FLI + std::string(name) + " expected a macro and a number of times to run it",
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
				return Frame::Exit(Frame::Exit::ERROR, "In Repeat Loop", DEBUG_FLI, f.feed.lineNumber(), ev);
		}

		// merge stacks
		f.stack = loop.stack;

		return Frame::Exit();

	}
}

Frame::Exit runMacro(Frame& f, const std::string& macro, const bool merge_stack = true) {
	Frame block = f.scope(CodeFeed(macro), merge_stack);
	Frame::Exit ev = block.run();
	if (merge_stack)
		f.stack = block.stack;
	return ev;
}

Frame::Exit runLambda(Frame& f, const Value& lam, const bool merge_stack = true) {
	return Frame::Exit();
}

namespace op_exec {
	const char* name = "@";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " @ operator expected something to run", f.feed.lineNumber());

		// get top
		const Value* ref = f.stack.back().defer();
		if (!ref)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " null/cyclic reference passed to @ operator", f.feed.lineNumber());
		Value v = *ref; // copy in case of race condition
		f.stack.pop_back();
		if (v.type == Value::MAC || v.type == Value::STR) {
			const Frame::Exit ev = runMacro(f, *v.str, true);
			if (ev.reason == Frame::Exit::ERROR)
				return Frame::Exit(Frame::Exit::ERROR,
						DEBUG_FLI "In " + std::string(v.typeName()) + " @ ", "", f.feed.lineNumber(), ev);

		} else if (v.type == Value::LAM) {
			std::cout <<"lambda exec not implemented\n";
		} else {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "non-exectuteable type (" + std::string(v.typeName()) + ") passed to @ operator", f.feed.lineNumber());
		}

		return Frame::Exit();
	}
}



/*
* {
* 	{ else action }
* 	{ elif action } elif cond
* 	...
* 	{ if action } if cond
* } cond
*
 * 1. run cond macro
 * 2. go through stack
 * 3. until you get to else clause
 * 4. if top value is truthy run macro and finish
 * 6. else pop it and its action and goto (3)
 */
namespace op_cond {
	const char* name = "cond";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act (Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI, f.feed.lineNumber());
		if (f.stack.back().type != Value::MAC)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " " + std::string(name) + " expected, a macro containing macros and conditions.", f.feed.lineNumber());

		// run it
		// go cycle through stack
		// 1: if condition is true
			// run; return
		// if condition is false
			// pop
		// if top == macro
			// run; return
		// if top == cond
			// goto 1

		Frame cond = f.scope(*f.stack.back().str, false);
		cond.feed.body = *f.stack.back().str;
		f.stack.pop_back();

		const Frame::Exit ev = cond.run();
		if (ev.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "In cond", DEBUG_FLI, f.feed.lineNumber(), ev);

		while (!cond.stack.empty()) {

			// if not an else clause
			if (cond.stack.size() > 1) {

				// is top value truthy?
				bool truthy = ((Value) *cond.stack.back().defer()).truthy();
				cond.stack.pop_back();

				// this isnt the one to run, skip it
				if (!truthy) {
					cond.stack.pop_back();
					continue;
				}
				// else, procede to run it
			}


			const Value* v = cond.stack.back().defer();
			if (v && v->type == Value::MAC) {

				Frame action = f.scope(*v->str);
				const Frame::Exit ev = action.run();
				if (ev.reason == Frame::Exit::ERROR)
					return Frame::Exit(Frame::Exit::ERROR, "In Cond", DEBUG_FLI, f.feed.lineNumber(), ev);
				// merge stack
				f.stack.insert(f.stack.end(), cond.stack.begin(), cond.stack.end());

			} else {
				f.stack.emplace_back(cond.stack.back());
			}

		}

		return Frame::Exit();
	}
}