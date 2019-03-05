
#include <algorithm>
#include "stack_ctl.hpp"


namespace op_stk_clear {
	const char* name = "...";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		f.stack.clear();
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
		if (f.stack.size() < 2)
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
		if (!f.stack.empty())
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

Frame::Exit clear_stack(Frame& f) {
	f.stack.clear();
	return Frame::Exit();
}
Frame::Exit dup_top(Frame& f) {
	f.stack.emplace_back(f.stack.back());
	return Frame::Exit();
}
Frame::Exit swap_top(Frame& f) {
	if (f.stack.size() < 2)
		return Frame::Exit(Frame::Exit::ERROR, "ArgError",
						   DEBUG_FLI + "stack :swap expected an 2 items to swap",
						   f.feed.lineNumber());

	std::swap(f.stack.back(), f.stack[f.stack.size() - 2]);
	return Frame::Exit();
}
Frame::Exit pop_top(Frame& f) {
	if (!f.stack.empty())
		f.stack.pop_back();
	return Frame::Exit();
}
Frame::Exit stack_size(Frame& f) {
	f.stack.emplace_back(f.stack.size());
	return Frame::Exit();
}

Frame::Exit reverse_stack(Frame& f) {
	std::reverse(f.stack.begin(), f.stack.end());
	return Frame::Exit();
}

Frame::Exit stack_list(Frame& f) {
	std::vector<std::shared_ptr<Value>> ret;

	for (Value& v : f.stack)
		ret.emplace_back(std::make_shared<Value>(v));
	f.stack.emplace_back(ret);
	return Frame::Exit();
}


const Namespace stack_ns({
		{ std::string("clear"),	Def(clear_stack)	},
		{ std::string("dup"),	Def(dup_top)		},
		{ std::string("swap"),	Def(swap_top)		},
		{ std::string("pop"),	Def(pop_top)		},
		{ std::string("size"),	Def(stack_size)		},
		{ std::string("reverse"), Def(reverse_stack)},
		{ std::string("list"),	Def(stack_list)		},
});


namespace op_stack {
	const char* name = "stack";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		f.stack.emplace_back(stack_ns);
		return Frame::Exit();
	}
}
