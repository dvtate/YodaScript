//
// Created by tate on 01-03-19.
//

#include "data_types.hpp"


/*

{
 	"label" value
 	"@label" executeable
} namespace

 */

namespace op_namespace {
	const char* name = "namespace";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += strlen(name);
		if (f.stack.empty() || f.stack.back().type != Value::MAC)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "namespace expected a macro containing elements", f.feed.lineNumber());

		Frame ns_body = f.scope(CodeFeed(*f.stack.back().str), false);
		Frame::Exit e = ns_body.run();
		if (e.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "in namespace", DEBUG_FLI, f.feed.lineNumber(), e);

		f.stack.back().set(ns_body.defs);
		return Frame::Exit();

	}
}

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

		const std::string name = f.feed.tok.substr(1, f.feed.tok.length());
		auto d = v->ns->find(name);

		f.stack.back().set(d == v->ns->end() ? Value() : Value(d->second));

		return Frame::Exit();

	}
}

namespace op_colon_op {

}
namespace op_def {
	const char* name = "def";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}
	Frame::Exit act(Frame& f) {
		f.feed.offset += f.feed.tok.length();
		if (f.stack.size() < 2)
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI " def expected a string label and value", f.feed.lineNumber());

		Value v = f.stack.back();
		f.stack.pop_back();
		if (f.stack.back().type != Value::STR)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI " def expected a string label and value", f.feed.lineNumber());

		// performance for normal users hurt for the benefit of inexperienced ones
		for (const char& c : *f.stack.back().str)
			if (c == ' ')
				return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI " def labels cannot have spaces", f.feed.lineNumber());

		// empty string label
		if (f.stack.back().str->empty())
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI " def label cannot be empty", f.feed.lineNumber());

		bool runnable = f.stack.back().str->at(0) == '@';
		std::string label = runnable ? f.stack.back().str->c_str() + 1 : *f.stack.back().str;
		f.stack.pop_back();

		f.defs.emplace(label, Def(v, runnable));

		return Frame::Exit();
	}
}