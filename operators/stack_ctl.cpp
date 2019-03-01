
#include <algorithm>
#include "stack_ctl.hpp"


namespace op_stk_clear {
	const char* name = "...";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.stack.clear();
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}
}
namespace op_stk_dup {
	const char* name = "dup";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (!f.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError",
					DEBUG_FLI + std::string(name) + " expected an element on to duplicate",
					f.feed.lineNumber());

		f.stack.push_back(f.stack.back());
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}
}
namespace op_stk_swap {
	const char* name = "swap";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (!f.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError",
							   DEBUG_FLI + std::string(name) + " expected an 2 items to swap",
							   f.feed.lineNumber());

		std::swap(f.stack.back(), f.stack[f.stack.size() - 2]);
		return Frame::Exit();
	}
}

namespace op_stk_pop {
	const char* name = ";";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size())
			f.stack.pop_back();
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}
}

namespace op_stk_size {
	const char* name = "stack_size";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(mpz_class(f.stack.size()));
		f.feed.offset += strlen(name);
		return Frame::Exit();
	}
}