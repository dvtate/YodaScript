//
// Created by tate on 09-03-19.
//

#include "generic_operators.hpp"


namespace op_size {
	const char* name = "size";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " size expected a sized value", f.feed.lineNumber());

		const Value* v = f.stack.back().defer();
		if (!v)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " null/cyclic reference passed to size", f.feed.lineNumber());
		//std::cout <<"size(" <<((Value*)v)->typeName() <<")";
		switch (v->type) {
			case Value::OBJ: {
				Frame::Exit ev;
				if (!v->obj->callMember(f, "__size", ev, f.stack.back().lastRef()))
					return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " object without overloaded self.__size", f.feed.lineNumber());
				return ev;
			}

			case Value::ARR:
				f.stack.back().set(mpz_class(v->arr->size()));
				break;
			case Value::STR: case Value::MAC:
				f.stack.back().set(mpz_class(v->str->length()));
				break;

			default:
				return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " size expected a sized value", f.feed.lineNumber());
		}

		return Frame::Exit();
	}
}

namespace op_keys {
	const char* name = "keys";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::Reason::ERROR, "ArgError", DEBUG_FLI " keys expected a value to key");

		const Value* v = f.stack.back().defer();
		std::vector<std::shared_ptr<Value>> keys;
		if (v->type == Value::OBJ) {
			keys.reserve(v->obj->members.size());
			for (const auto& p : v->obj->members)
				keys.emplace_back(std::make_shared<Value>(p.first));

		} else if (v->type == Value::NSP) {
			keys.reserve(v->ns->size());
			for (const auto& p : *v->ns)
				keys.emplace_back(std::make_shared<Value>(p.first));

		} else {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " keys called on non-keyable value");
		}

		f.stack.back().set(keys);

		return Frame::Exit();
	}
}