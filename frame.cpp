
#include <ctype.h>
#include "namespace_def.hpp"
#include "operators.hpp"
#include "frame.hpp"

Frame::Frame(){
	stack.reserve(30);
}

Frame::Frame(const CodeFeed& cf): feed(cf) {
	stack.reserve(30);
}

Frame::Exit Frame::runDef(const Def& def) {
	if (def.native && def.act) {
		return def.act(*this);

	} else if (def.run) {

		stack.emplace_back(*def._val);
		Frame::Exit exit;
		operators::callByName(*this, "@", exit);
		return exit;

	} else {
		stack.emplace_back(*def._val);
		return Frame::Exit();
	}
}
inline bool check_def(Frame& f, Frame::Exit& ev) {
	if (operators::callOperator(f, ev, f.defs))
		return true;
	for (Frame* fp : f.prev)
		if (operators::callOperator(f, ev, fp->defs))
			return true;

	return false;
}

Frame::Exit Frame::run() {
	//std::cout <<"running line: " <<feed.body <<std::endl;

	Frame::Exit ev;


	if (prev.size() > 2000) {
		std::cerr <<"Maximum scope depth reached!!! generating backtrace... (you may run out of ram, probably best to kill w/ ctl+c)" <<std::endl;
		Frame::Exit ev = Frame::Exit(Frame::Exit::Reason::ERROR, "ScopeError", "Maximum recursion depth reached", feed.lineNumber());
		ev.genMsg(feed, this);
		std::cerr <<"Error(" <<prev.size() <<"): " <<ev.msg;
		return Frame::Exit(Frame::Exit::Reason::ERROR, "ScopeError", "Maximum recursion depth reached", feed.lineNumber());
	}
run_frame:
	do {
		// defs get automatically evaluated
		while (!stack.empty() && ev.reason == Frame::Exit::CONTINUE && stack.back().type == Value::DEF) {
			Def d = *stack.back().def;
			//std::cout <<Value(d).depict() <<std::endl;
			stack.pop_back();
			ev = runDef(d);
		}

		// get first token once so that we dont have to find it for every operator
		// stored in feed.tok
		if (!feed.setTok())
			return Frame::Exit(Frame::Exit::CONTINUE);
		feed.offset += feed.tok.length();

		// user-level definitions & imports > interpreter level operators > interpreter level tokens
		if (!check_def(*this, ev) && !operators::callOperator(*this, ev)) {
			// wasn't an operator so it's prolly a token
			feed.offset -= feed.tok.length();
			if (!operators::callToken(*this, ev)) {
				ev = Frame::Exit(Frame::Exit::ERROR, "SyntaxError",
								 "unknown token near `" + feed.tok + "`\n", feed.lineNumber());
				break;
			}
		}


	} while (ev.reason == Frame::Exit::CONTINUE);

	if (ev.reason == Frame::Exit::UP) {
		if (ev.number == 0)
			goto run_frame;
		ev.number--;
	}

	if (ev.reason == Frame::Exit::ERROR)
		ev.genMsg(feed, this);

	return ev;

}

std::shared_ptr<Value> Frame::getVar(const std::string& name) {
	std::shared_ptr<Value> v = findVar(name);
	if (!v) {
		// make a new empty value to point to
		std::shared_ptr<Value> e = std::make_shared<Value>();
		vars.emplace(name, e);
		ref_vals.emplace_back(e);
		return e;
	}
	return v;
}

// searches for var, if its found in previous scope, make a reference to it in current scope
std::shared_ptr<Value> Frame::findVar(const std::string& name) {
	auto v = vars.find(name);
	if (v != vars.end())
		return *v->second.ref;

	// check previous scopes
	for (Frame* scope : prev) {
		v = scope->vars.find(name);
		if (v != scope->vars.end()) {
			// set it to a ref to tht var's value (double reference)
			std::shared_ptr<Value> r = std::make_shared<Value>(v->second);
			vars.emplace(name, r);
			ref_vals.emplace_back(r);
			return r;
		}
	}
	return nullptr;
}

// set var in current scope
std::shared_ptr<Value> Frame::setVar(const std::string& name, const std::shared_ptr<Value>& val) {

	auto v = vars.find(name);
	if (v != vars.end())
		return *v->second.ref = val;

	vars.emplace(name, val);
	v = vars.find(name);
	return *v->second.ref;
}

Frame Frame::scope(const CodeFeed& feed, bool copy_stack) {
	Frame ret(feed);
	ret.prev.emplace_back(this);
	for (Frame* f : prev)
		ret.prev.emplace_back(f);

	if (copy_stack)
		ret.stack = stack; // copy stack

	return ret;
}
