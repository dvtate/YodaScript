//
// Created by tate on 24-02-19.
//


#include "types.hpp"


namespace op_typeof {
	const char* name = "type";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (!f.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + "expected a value to find type of", f.feed.lineNumber());

		f.stack.back().set(f.stack.back().typeName());
		return Frame::Exit();
	}
}

namespace op_str {
	const char* name = "str";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to stringify", f.feed.lineNumber());

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			if (!v->obj->callMember(f, "__str", ev, self))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__str", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		f.stack.back().set(f.stack.back().toString());
		return Frame::Exit();
	}
}

namespace op_depict {
	const char* name = "depict";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to depict", f.feed.lineNumber());

		const Value* v = f.stack.back().defer();
		if (v && v->type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			if (!v->obj->callMember(f, "__depict", ev, self))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__depict", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		}

		f.stack.back().set(f.stack.back().depict());
		return Frame::Exit();
	}
}

namespace op_int {
	const char* name = "int";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to convert to an int", f.feed.lineNumber());

		DEFER_TOP(f);
		if (f.stack.back().type == Value::STR) {
			try {
				f.stack.back().set(mpz_class(*f.stack.back().str));
			} catch (const std::invalid_argument&) {
				return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " int received an invalid string", f.feed.lineNumber());
			}
		} else if (f.stack.back().type == Value::INT) {
			return Frame::Exit();
		} else if (f.stack.back().type == Value::DEC) {
			try {
				f.stack.back().set(mpz_class(f.stack.back().dec));
			} catch (const std::invalid_argument &) {
				return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " int received an invalid float",
								   f.feed.lineNumber());
			}
		} else if (f.stack.back().type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value>&& self = f.stack.back().lastRef();
			if (!f.stack.back().obj->callMember(f, "__int", ev, self))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__int", DEBUG_FLI , f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		} else
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " invalid " + std::string(f.stack.back().typeName()) + " passed to int" );

		return Frame::Exit();
	}
}

namespace op_float {
	const char* name = "float";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI + std::string(name) + " expected a value to convert to float", f.feed.lineNumber());

		DEFER_TOP(f);
		if (f.stack.back().type == Value::STR) {
			f.stack.back().set(std::stod(*f.stack.back().str));
		} else if (f.stack.back().type == Value::INT) {
			f.stack.back().set(f.stack.back().mp_int->get_d());
		} else if (f.stack.back().type == Value::OBJ) {
			Frame::Exit ev;
			const std::shared_ptr<Value> &&self = f.stack.back().lastRef();
			if (!f.stack.back().obj->callMember(f, "__float", ev, self))
				return Frame::Exit(Frame::Exit::ERROR, "in object.__float", DEBUG_FLI, f.feed.lineNumber(), ev);
			if (ev.reason == Frame::Exit::RETURN)
				return Frame::Exit();
			return ev;
		} else if (f.stack.back().type == Value::DEC) {
			;
		} else
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "invalid type " + std::string(f.stack.back().typeName()) + " passed to float" );

		
	}
}
