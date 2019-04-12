
#include <algorithm>
#include "stack_ctl.hpp"


namespace op_stk_pop {
	const char* name = ";";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (!f.stack.empty())
			f.stack.pop_back();
		return Frame::Exit();
	}
}


namespace stack {

	Frame::Exit clear(Frame& f) {
		f.stack.clear();
		return Frame::Exit();
	}
	Frame::Exit dup(Frame& f) {
		f.stack.emplace_back(f.stack.back());
		return Frame::Exit();
	}
	Frame::Exit swap(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError",
							   DEBUG_FLI + "stack :swap expected an 2 items to swap",
							   f.feed.lineNumber());

		std::swap(f.stack.back(), f.stack[f.stack.size() - 2]);
		return Frame::Exit();
	}
	Frame::Exit pop(Frame& f) {
		if (!f.stack.empty())
			f.stack.pop_back();
		return Frame::Exit();
	}
	Frame::Exit size(Frame& f) {
		f.stack.emplace_back(f.stack.size());
		return Frame::Exit();
	}

	Frame::Exit reverse(Frame& f) {
		std::reverse(f.stack.begin(), f.stack.end());
		return Frame::Exit();
	}

	Frame::Exit list(Frame& f) {
		std::vector<std::shared_ptr<Value>> ret;

		for (Value& v : f.stack)
			ret.emplace_back(std::make_shared<Value>(v));
		f.stack.emplace_back(ret);
		return Frame::Exit();
	}

	const Namespace stack_ns = {
		{ std::string("clear"),		Def(clear)	},
		{ std::string("top"),		Def(dup)	},
		{ std::string("swap"),		Def(swap)	},
		{ std::string("pop"),		Def(pop)	},
		{ std::string("size"),		Def(size)	},
		{ std::string("reverse"),	Def(reverse)},
		{ std::string("list"),		Def(list)	},
	 };

}



namespace op_stack {
	const char* name = "Stack";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(stack::stack_ns);
		return Frame::Exit();
	}
}
