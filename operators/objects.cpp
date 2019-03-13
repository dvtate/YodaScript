
#include "objects.hpp"


namespace op_object {
	const char* name = "object";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "Object expected a macro initializer", f.feed.lineNumber());

		if (f.stack.back().type != Value::MAC)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Object expected a macro initializer", f.feed.lineNumber());

		std::shared_ptr<Value> obj = std::make_shared<Value>(Object());
		Frame init = f.scope(*f.stack.back().str, false);
		init.defs.emplace("self", obj);
		f.stack.pop_back();
		const Frame::Exit ev = init.run();
		if (ev.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "In object Initializer", DEBUG_FLI, f.feed.lineNumber(), ev);
		if (init.stack.size() % 2 != 0)
			return Frame::Exit(Frame::Exit::ERROR, "invalid object initializer", "expected pairs of members and values", f.feed.lineNumber());

		for (size_t i = 0; i < init.stack.size(); i += 2) {
			Value* p_ref = init.stack[i].deferMuteable();
			if (!p_ref)
				return Frame::Exit(Frame::Exit::ERROR, "RefError", DEBUG_FLI " In object initializer: null or cyclic reference", f.feed.lineNumber());
			p_ref->set(init.stack[i + 1]);
		}

		f.stack.emplace_back(obj);

		return Frame::Exit();
	}

}

namespace op_obj_mem_acc {
	const char* name = ".*";
	bool condition(Frame& f) {
		return f.feed.tok.at(0) == '.';
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += f.feed.tok.length();

		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "Object member accessor without object", f.feed.lineNumber());

		Value* v = (Value*) f.stack.back().defer();


		// if empty, make it an object
		if (!v || v->type != Value::OBJ) {
			v = f.stack.back().deferMuteable();
			if (!v)
				return Frame::Exit(Frame::Exit::ERROR, "ReferenceError", DEBUG_FLI "cannot get member of object at Null/Cyclic reference", f.feed.lineNumber());

			v->set(Object());
		}
		auto mem = v->obj->getMember(f.feed.tok.substr(1));
		f.stack.back().set(Value(mem, f.stack.back().lastRef()));

		return Frame::Exit();

	}
}


namespace op_obj_mem_acc_op {
	const char* name = ".";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " . operator expected a namespace and string",
							   f.feed.lineNumber());

		Value* v = (Value*) f.stack.back().defer();
		std::string key;
		if (v->type == Value::OBJ) { // they're requesting val at emptystring key
			key = "";
		} else if (v->type != Value::STR) { // $namespace (1,2,3) :  (not a string)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " . operator expected a string key");
		} else { // normative
			key = *v->str;
			f.stack.pop_back();
			v = (Value*) f.stack.back().defer();
			//if (v->type != Value::OBJ)
			//	return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " . operator expected an object to act on");
		}

		// if empty, make it an object
		if (v->type != Value::OBJ) {
			v = f.stack.back().deferMuteable();
			v->set(Object());
		}

		auto mem = v->obj->getMember(f.feed.tok.substr(1));
		// reference the object member with the object as a dependency
		f.stack.back().set(Value(mem, f.stack.back().lastRef()));


		return Frame::Exit();
	}
}
