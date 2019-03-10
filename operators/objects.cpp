
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

		/*TODO: FIXME: this muteable/immutable refs and stuff
			error messages also kinda important since u shouldn't
			be able to convert anything past an immuteable ref into an object

		 	this is gonna actually be rly painful without lazy evaluation
		 */
		Value* v = (Value*) f.stack.back().defer();

		// if empty, make it an object
		if (v->type != Value::OBJ)
			v->set(Object());

		auto mem = v->obj->getMember(f.feed.tok.substr(1));
		f.stack.back().set(Value(mem, f.stack.back().lastRef()));

		return Frame::Exit();

	}
}