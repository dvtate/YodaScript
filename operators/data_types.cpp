//
// Created by tate on 01-03-19.
//

#include "data_types.hpp"

/*

{
 	"label" value
 	"@label" executeable
} `namespace`

 */
namespace op_namespace {
	const char* name = "namespace";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty() || f.stack.back().type != Value::MAC)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "namespace expected a macro containing elements", f.feed.lineNumber());

		std::shared_ptr<Frame> ns_body = f.scope(CodeFeed(*f.stack.back().str), false);
		Frame::Exit e = ns_body->run(ns_body);
		if (e.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "in namespace", DEBUG_FLI, f.feed.lineNumber(), e);

		f.stack.back().set(ns_body->defs);
		return Frame::Exit();

	}
}

// $nsp `:member`
namespace op_ns_member_req {
	const char* name = ":*";
	bool condition(Frame& f) {
		return f.feed.tok[0] == name[0];
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += f.feed.tok.length();
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " namespace member accessor requires a namespace to act on", f.feed.lineNumber());

		const Value* v = f.stack.back().defer();
		if (v->type != Value::NSP)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " namespace member accessor requires a namespace to act on (received " + std::string(((Value*)v)->typeName()), f.feed.lineNumber());

		// lookup key
		auto d = v->ns->find(std::move(f.feed.tok.substr(1, f.feed.tok.length())));

		// if found, replace top w/ it, else replace top w/ empty
		f.stack.back().set(d == v->ns->end() ? Value() : Value(d->second));

		return Frame::Exit();
	}
}


// $nsp "member" `:`
namespace op_ns_mem_req_op {
	const char* name = ":";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " : operator expected a namespace and string",
							   f.feed.lineNumber());

		Value* v = (Value*) f.stack.back().defer();
		std::string key;
		if (v->type == Value::NSP) { // they're requesting val at emptystring key
			key = "";
		} else if (v->type != Value::STR) { // $namespace (1,2,3) :  (not a string)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " : operator expected a string key");
		} else { // normative
			key = *v->str;
			f.stack.pop_back();
			v = (Value*) f.stack.back().defer();
			if (v->type != Value::NSP)
				return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " : operator expected a namespace to act on");
		}

		// lookup
		auto d = v->ns->find(key);
		// put on stack
		f.stack.back().set(d == v->ns->end() ? Value() : Value(d->second));

		return Frame::Exit();
	}
}

// "pi" 3.14 define
namespace op_def {
	const char* name = "define";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " define expected a string label and value", f.feed.lineNumber());

		Value v = f.stack.back();
		f.stack.pop_back();
		if (f.stack.back().type != Value::STR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " define expected a string label and value", f.feed.lineNumber());

		// empty string label
		/*if (f.stack.back().str->empty())
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI " define label cannot be empty", f.feed.lineNumber());
		*/
		bool runnable = !f.stack.back().str->empty() && f.stack.back().str->at(0) == '@';
		std::string label = runnable ? f.stack.back().str->c_str() + 1 : *f.stack.back().str;
		f.stack.pop_back();

		// TODO: make it so user can redefine using same operator
		f.defs.emplace(label, Def(v, runnable));
		
		/*
		Def d(v, runnable);

		f.defs[label] = d;
		std::cout <<"f[l]:" <<Value(f.defs[label]).depict() <<" d" <<Value(d).depict() <<std::endl;

		auto p = f.defs.find(label);
		std::cout <<p->first <<" -> " <<Value(p->second).depict() <<std::endl;
		*/
		return Frame::Exit();
	}
}

/*

# original syntax
$add {
	5 return
} ($key, $value) lambda =

# temporary syntax
$add {
	5 return
} ('key', 'value') lambda =

* lists of references will not be able to remember the variable
* names so i gotta either change the way lists get parsed, or come up
* with a different syntax
*/
namespace op_lambda {

	const char* name = "lambda";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {

		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " lambda expected a macro body and a list of parameters", f.feed.lineNumber());

		if (f.stack.back().type != Value::ARR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " expected a parameters list", f.feed.lineNumber());

		Lambda v;

		for (const auto& param : *f.stack.back().arr) {
			if (!param)
				return Frame::Exit(Frame::Exit::ERROR, "ValueError", DEBUG_FLI " lambda params list contained null value and not string variable names", f.feed.lineNumber());
			if (param->type == Value::EMT)
				continue;
			if (param->type != Value::STR)
				return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI " params list should contain strings for variable names", f.feed.lineNumber());

			v.params.push_back(*param->str);
		}
		f.stack.pop_back();

		if (f.stack.back().type != Value::MAC)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " lambda expected a macro body, " + std::string(f.stack.back().typeName()) + " received", f.feed.lineNumber());
		v.body = *f.stack.back().str;

		f.stack.back().set(v);

		return Frame::Exit();
	}

}