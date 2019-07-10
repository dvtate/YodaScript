
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
		std::shared_ptr<Frame> init = f.scope(*f.stack.back().str, false);
		init->defs.emplace("self", obj);
		f.stack.pop_back();

		const Frame::Exit ev = init->run(init);
		if (ev.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "In object Initializer", DEBUG_FLI, f.feed.lineNumber(), ev);
		if (init->stack.size() % 2 != 0)
			return Frame::Exit(Frame::Exit::ERROR, "invalid object initializer", "expected pairs of members and values", f.feed.lineNumber());

		// initiaize object
		for (size_t i = 0; i < init->stack.size(); i += 2) {
			Value* p_ref = init->stack[i].deferMuteable();
			if (!p_ref)
				return Frame::Exit(Frame::Exit::ERROR, "RefError", DEBUG_FLI " In object initializer: null or cyclic reference", f.feed.lineNumber());
			p_ref->set(init->stack[i + 1]);
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
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " . operator expected an object and string",
							   f.feed.lineNumber());

		Value* v = (Value*) f.stack.back().defer();

		if (v->type != Value::STR) // $obj (1,2,3) .  (not a string)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " . operator expected a string key");
		// normative
		std::string key = *v->str;
		f.stack.pop_back();
		v = (Value*) f.stack.back().defer();

		// TODO: come up with a system to prevent changes to immuteable referenced values
		// TODO: don't allow . operator on empty value

		//if (v->type != Value::OBJ)
		//	return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " . operator expected an object to act on");

		// if empty, make it an object
		if (v->type != Value::OBJ) {
			v = f.stack.back().deferMuteable();
			v->set(Object());
		}

		auto mem = v->obj->getMember(key);

		// reference the object member with the object as a dependency
		f.stack.back().set(Value(mem, f.stack.back().lastRef()));
/*
		// reference the object member with the object as a dependency
		if (mem.type == Value::REF)
			mem.related = new std::shared_ptr<Value>(f.stack.back().lastRef());

		f.stack.back().set(mem);
*/
		return Frame::Exit();
	}
}

namespace ns_object {

	const char* name = "Object";

	Frame::Exit keys(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "Object:keys expected an object to key", f.feed.lineNumber());

		DEFER_TOP(f);

		if (f.stack.back().type != Value::OBJ)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Object:keys expected an object to act on", f.feed.lineNumber());

		std::vector<std::shared_ptr<Value>> ret;
		ret.reserve(f.stack.back().obj->members.size());
		for (const auto& m : f.stack.back().obj->members)
			ret.push_back(std::make_shared<Value>(m.first));

		f.stack.back().set(ret);

		return Frame::Exit();

	}

	Frame::Exit inherit(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "Object:inherit expected an object reference and an object to copy the members of", f.feed.lineNumber());

		DEFER_TOP(f);
		if (f.stack.back().type != Value::OBJ)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Object:inherit expectd an object to copy the members of (recieved: " + std::string(f.stack.back().typeName()) +")", f.feed.lineNumber());

		Value* oref = f.stack.back().deferChange();

		for (auto m : f.stack.back().obj->members)
			;

		return Frame::Exit();
	}

	const Namespace object_ns = {
		{ "inherit", inherit },
		{ "keys", keys 		 }
	};

	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(object_ns);
		return Frame::Exit();
	}
}