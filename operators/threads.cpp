//
// Created by tate on 13-03-19.
//

#include "threads.hpp"

namespace op_quit {
	const char* name = "exit";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		const Value* v = f.stack.empty() ? nullptr : f.stack.back().defer();
		if (f.feed.isStdin)
			if (v && v->type == Value::INT)  // trusting user to provide reasonable value
				exit(v->mp_int->get_si());	// worst case: weird/unexpected exit code
			else
				exit(0);
		else
			f.feed.offset = f.feed.body.length() - 1;

		return Frame::Exit(Frame::Exit::FEED_END);
	}
}