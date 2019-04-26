//
// Created by tate on 02-03-19.
//

#include "list_stuff.hpp"

namespace op_index {
	inline size_t normalize_index(const Value& ind, const size_t list_size) {
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
			if (ind.type == Value::INT)
				return *ind.mp_int < 0 ? ind.mp_int->get_si() + list_size : ind.mp_int->get_ui();
			else if (ind.type == Value::DEC)
				return (size_t) (ind.dec < 0 ? ind.dec + list_size : ind.dec);
			else
				return list_size + 1;
		};

		const Value ind = f.stack.back();
		f.stack.pop_back();

		DEFER_TOP(f);
		if (f.stack.back().type != Value::ARR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "can only index] lists currently. received: " + std::string(f.stack.back().typeName()), f.feed.lineNumber());

		// support negative indicies
		const size_t i = normalize_index(ind, f.stack.back().arr->size());

		if (i >= f.stack.back().arr->size())
			return Frame::Exit(Frame::Exit::ERROR, "IndexError", DEBUG_FLI "list index out of bounds (ind: " + std::to_string(i) + " , size: " + std::to_string(f.stack.back().arr->size()) + ")", f.feed.lineNumber());

		f.stack.back().set(f.stack.back().arr->at(i));

		return Frame::Exit();
	}
}

namespace op_list_ns {
	const char* name = "List";

	Frame::Exit pop(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "List:pop expected a list to act on", f.feed.lineNumber());

		const Value* v = f.stack.back().deferChange();
		if (v->type != Value::ARR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "List:pop expected a list to act on", f.feed.lineNumber());

		f.stack.back().set(v->arr->back());
		v->arr->pop_back();
		return Frame::Exit();
	}


	Frame::Exit push(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "List:push expected a list and a value", f.feed.lineNumber());

		Value v = f.stack.back();
		f.stack.pop_back();
		Value* l = f.stack.back().deferChange();

		if (l->type != Value::ARR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "List:push expected a list but received " + std::string(l->typeName()), f.feed.lineNumber());

		l->arr->emplace_back(std::make_shared<Value>(v));
		f.stack.pop_back();

		return Frame::Exit();
	}

	Frame::Exit for_each(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " list:foreach expected a list and a lambda");

		DEFER_TOP(f)
		const Value list = f.stack.back();
		f.stack.pop_back();
		Value lam;
		const bool ref = f.stack.back().deferValue(lam);
		// get top
		if (!ref)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " null/cyclic reference passed to list:foreach operator (expected lambda)", f.feed.lineNumber());

		if (lam.type != Value::LAM)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " list:foreach expected a lambda, received " + std::string(lam.typeName()), f.feed.lineNumber());

		f.stack.pop_back();

		Value arg_list;
		arg_list.set(std::vector<std::shared_ptr<Value>>());
		arg_list.arr->emplace_back(std::shared_ptr<Value>());

		for (const std::shared_ptr<Value>& e : *list.arr) {
			arg_list.arr->at(0) = e;
			f.stack.emplace_back(arg_list);

			Frame::Exit ev = lam.lam->call(f);
			if (ev.reason == Frame::Exit::ERROR)
				return Frame::Exit(Frame::Exit::ERROR, "In lambda @", DEBUG_FLI, f.feed.lineNumber(), ev);

			if (ev.reason == Frame::Exit::UP) {
				ev.number--;
				return ev;
			}
			if (ev.reason == Frame::Exit::ESCAPE)
				return ev;

		}

		return Frame::Exit();
	}

	const Namespace list_ns = {
		{ "pop", pop },
		{ "push", push },
		{ "for_each", for_each },
	};

	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(list_ns);
		return Frame::Exit();
	}
}

/* list namespace
 * :pop
 * :push
 * :size
 *
 * :map
 * :foreach
 * :find
 * :sort
 * :max, :min ?
 *
 */