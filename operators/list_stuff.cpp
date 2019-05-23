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
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " list:foreach expected a list and a lambda", f.feed.lineNumber());

		DEFER_TOP(f);
		const Value list = f.stack.back();
		f.stack.pop_back();
		Value lam;
		const bool ref = f.stack.back().deferValue(lam);

		// get top
		if (!ref)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " null/cyclic reference passed to list:for_each operator (expected lambda)", f.feed.lineNumber());

		if (lam.type != Value::LAM)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " list:foreach expected a lambda, received " + std::string(lam.typeName()), f.feed.lineNumber());

		f.stack.pop_back();

		Value arg_list;
		arg_list.set(std::vector<std::shared_ptr<Value>>());
		arg_list.arr->resize(3);

		// { } ($element, $index, $source_list) lambda
		arg_list.arr->at(2) = std::make_shared<Value>(list);

		for (size_t i = 0; i < list.arr->size(); i++) {
			arg_list.arr->at(0) = list.arr->at(i);
			arg_list.arr->at(1) = std::make_shared<Value>(mpz_class(i));

			f.stack.emplace_back(arg_list);

			Frame::Exit ev = lam.lam->call(f);
			if (ev.reason == Frame::Exit::ERROR) {
				return Frame::Exit(Frame::Exit::ERROR, "In List:for_each", DEBUG_FLI + "Index - " + std::to_string(i),
								   f.feed.lineNumber(), ev);

			}
			if (ev.reason == Frame::Exit::UP) {
				ev.number--;
				return ev;
			}
			if (ev.reason == Frame::Exit::ESCAPE)
				return ev;

		}

		return Frame::Exit();
	}

	Frame::Exit map(Frame& f) {
		// similar to for_each but in new scope, also capturing return values
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " List:map expected a list and a lambda", f.feed.lineNumber());

		//const std::shared_ptr<Value> list_ref = f.stack.back().lastRef();

		DEFER_TOP(f);
		const Value list = f.stack.back();
		f.stack.pop_back();

		Value lam;
		const bool ref = f.stack.back().deferValue(lam);
		if (!ref)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " null/cyclic reference passed to List:map operator (expected lambda)", f.feed.lineNumber());
		if (lam.type != Value::LAM)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " List:map expected a lambda, received " + std::string(lam.typeName()), f.feed.lineNumber());

		f.stack.pop_back();

		std::shared_ptr<Frame> scope = f.scope(CodeFeed(), false);


		Value arg_list;
		arg_list.set(std::vector<std::shared_ptr<Value>>());
		arg_list.arr->resize(3);

		// { } ($element, $index, $source_list) lambda
		arg_list.arr->at(2) = std::make_shared<Value>(list);

		Value ret_list = std::vector<std::shared_ptr<Value>>();
		ret_list.arr->reserve(list.arr->size());


		for (size_t i = 0; i < list.arr->size(); i++) {
			// configure lambda arguments
			arg_list.arr->at(0) = list.arr->at(i);
			arg_list.arr->at(1) = std::make_shared<Value>(mpz_class(i));
			scope->stack.emplace_back(arg_list);



			// call lambda
			scope->self_ref = scope;
			Frame::Exit ev = lam.lam->call(*scope);
			scope->self_ref = nullptr;

			// early exit
			if (ev.reason == Frame::Exit::ERROR)
				return Frame::Exit(Frame::Exit::ERROR, "In List:map", DEBUG_FLI + " Index - " + std::to_string(i),
								   f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::UP) {
				ev.number--;
				return ev;
			}
			if (ev.reason == Frame::Exit::ESCAPE)
				break;



			// add return value to the return list
			ret_list.arr->emplace_back(scope->stack.empty() ?
										std::make_shared<Value>(Value())
										: std::make_shared<Value>(scope->stack.back()));

			scope->stack.clear(); // reduce memory consumption (at cost of cpu?)

		}

		f.stack.emplace_back(ret_list);

		return Frame::Exit();

	}


	Frame::Exit range(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "List:range expected a start and end", f.feed.lineNumber());

		ssize_t start, end;
		DEFER_TOP(f);
		if (f.stack.back().type == Value::INT) {

		} else if (f.stack.back().type == Value::DEC) {

		} else {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "List:range end range expected a number, received "
				+ std::string(f.stack.back().typeName()), f.feed.lineNumber());
		}
		DEFER_TOP(f);
		return Frame::Exit();
	}
	const Namespace list_ns = {
		{ "pop",	pop },
		{ "push",	push },
		{ "for_each", for_each },
		{ "map",	map },
	};

	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(list_ns);
		return Frame::Exit();
	}
}

/* list namespace
 * these could also be implemented in YodaScript
 * # :pop
 * # :push
 *
 * # :map
 * # :foreach
 * :filter
 * :find
 * :sort
 * :max, :min ?
 */