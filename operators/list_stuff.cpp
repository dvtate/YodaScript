//
// Created by tate on 02-03-19.
//

#include "list_stuff.hpp"

namespace op_index {
	inline size_t normalize_index(const Value& ind, size_t list_size) {
		bool neg;
		size_t i;
		if (ind.type == Value::INT) {
			neg = *ind.mp_int < 0;
			i = ind.mp_int->get_ui();
		} else {
			neg = ind.dec < 0;
			i = (size_t) (-ind.dec);
		}
		if (neg)
			return list_size - i; // ignoring overflow error because it should get handled later
		else
			return i;
	}

	const char* name = "]";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", " ] index lacks number or list", f.feed.lineNumber());
		DEFER_TOP(f);
		if (f.stack.back().type != Value::INT && f.stack.back().type != Value::DEC)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", " ] index expected a number + received: " + std::string(f.stack.back().typeName()), f.feed.lineNumber());

		auto normalize_ind = [](Value ind, size_t list_size){

		};
		const Value ind = f.stack.back();
		f.stack.pop_back();

		DEFER_TOP(f);
		if (f.stack.back().type != Value::ARR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", "can only index] lists currently. received: " + std::string(f.stack.back().typeName()), f.feed.lineNumber());

		// support negative indicies
		const size_t i = normalize_index(ind, f.stack.back().arr->size());

		if (i >= f.stack.back().arr->size())
			return Frame::Exit(Frame::Exit::ERROR, "IndexError", "list index out of bounds", f.feed.lineNumber());


		f.stack.back().set(f.stack.back().arr->at(i));

		return Frame::Exit();
	}
}

/* list namespace
 * :pop
 * :push
 *
 */