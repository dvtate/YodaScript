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

		Frame ns_elems = f.scope(CodeFeed(f.stack.back().str), false);
		Frame::Exit e = ns_elems.run();
		if (e.reason == Frame::Exit::ERROR)
			return Frame::Exit(Frame::Exit::ERROR, "in namespace", DEBUG_FLI, f.feed.lineNumber(), e);
		if (ns_elems.stack.size() % 2 != 0)
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", DEBUG_FLI "Namespace expects one key for every value", f.feed.lineNumber());

		Namespace ns;
		while (!ns_elems.stack.empty()) {
			// get value inside macro
			const Value v = ns_elems.stack.back();
			ns_elems.stack.pop_back();

			// get label
			if (ns_elems.stack.back().type != Value::STR)
				return Frame::Exit(Frame::Exit::ERROR, "in namespace", DEBUG_FLI " expected string label", f.feed.lineNumber());

			const bool runnable = ns_elems.stack.back().str->at(0) == '@';
			const std::string label = runnable ?
									  ns_elems.stack.back().str->c_str() + 1
											   : *ns_elems.stack.back().str;

			ns_elems.stack.pop_back();
			ns.emplace(label, Def(v, runnable));
		}


		f.stack.back() = ns;
		return Frame::Exit();


	}
}