//
// Created by tate on 22-02-19.
//

#include "ref_ops.hpp"

namespace op_var_literal {
	const char* name = "$*";
	bool condition(Frame& f) {
		return f.feed.tok[0] == '$';
	}
	Frame::Exit act(Frame& f) {
		// push corresponding reference onto stack
		f.stack.emplace_back(f.getVar(f.feed.tok.substr(1, f.feed.tok.length() - 1)));
		f.feed.offset += f.feed.tok.length();
		return Frame::Exit();
	}
}

namespace op_var_op {
	const char* name = "$";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (!f.stack.size() || f.stack.back().type != Value::STR)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " $ operator expected a string variable name", f.feed.lineNumber());

		f.stack.back() = Value(f.getVar(*f.stack.back().str));

		return Frame::Exit();
	}
}

namespace op_equals {
	const char* name = "=";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " = expected a reference and a value to assign", f.feed.lineNumber());


		Value v = f.stack.back();
		f.stack.pop_back();
		Value ref = f.stack.back();
		f.stack.pop_back();

		if (ref.type != Value::REF)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " = requires a reference to assign", f.feed.lineNumber());

		ref.deferMuteable()->set(v);

		return Frame::Exit();
	}
}

namespace op_set {
	const char* name = "set";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " set expected a reference and a value to assign", f.feed.lineNumber());

		Value v = f.stack.back();
		f.stack.pop_back();
		Value ref = f.stack.back();
		f.stack.pop_back();

		if (ref.type != Value::REF)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " set requires a reference to assign", f.feed.lineNumber());

		// if its a ref point to it,
		// else copy data into var data
		(*ref.ref)->set(v);

		return Frame::Exit();
	}
}

namespace op_copy_value {
	const char* name = "~";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (!f.stack.size())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " ~ expected a reference to defer", f.feed.lineNumber());


		const bool isNull = !f.stack.back().deferValue(f.stack.back());
		if (isNull)
			f.stack.back().set(nullptr);

		// deep copy array elems because they're references to originals
		if (f.stack.back().type == Value::ARR)
			for (std::shared_ptr<Value>& e : *f.stack.back().arr)
				e = std::make_shared<Value>(*e);

		return Frame::Exit();
	}
}

namespace op_vars {
	const char* name = "vars";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		std::cout <<"Current Scope:\n";
		std::cout <<"\tname\taddress\tvalue\n";
		for (auto& v : f.vars)
			//if ((*v.second.ref)->type != Value::EMT)
				std::cout <<"\t$" <<v.first <<'\t' <<*v.second.ref <<'\t' <<(*v.second.ref)->typeName() <<'\t' <<v.second.depict() <<std::endl;

		unsigned short bt = 1;
		for (const auto& pf : f.prev) {
			std::cout << "Previous scope " << bt++ << ":\n";
			for (auto &v : pf->vars)
				//if ((*v.second.ref)->type != Value::EMT)
					std::cout << "\t$" << v.first   <<'\t' <<*v.second.ref <<'\t' <<(*v.second.ref)->typeName() <<'\t' <<v.second.depict() <<std::endl;
		}

		return Frame::Exit();
	}
}

namespace op_const {
	const char* name = "weak";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "weak expected a reference to weaken", f.feed.lineNumber());


		std::shared_ptr<Value> v = std::make_shared<Value>(f.stack.back());

		f.stack.back() = Value(Value::IMR, v);

		return Frame::Exit();
	}
}


namespace op_var_name {
	const char* name = "var_name";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.stack.back().set(f.varName(*f.stack.back().ref));
		return Frame::Exit();
	}
}