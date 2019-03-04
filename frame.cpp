
#include <ctype.h>
#include "namespace.hpp"
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
		feed.offset--;
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
	do {
		// defs get automatically evaluated
		while (ev.reason == Frame::Exit::CONTINUE && stack.back().type == Value::DEF) {
			std::cout <<"def pop\n";
			ev = runDef(*stack.back().def);
			stack.pop_back();
		}

		// get first token once so that we dont have to find it for every operator
		// stored in feed.tok
		if (!feed.setTok())
			return Frame::Exit(Frame::Exit::FEED_END);

		//std::cout <<"tok" <<feed.tok<<std::endl;

		if (!check_def(*this, ev) && !operators::callOperator(*this, ev) && !operators::callToken(*this, ev)) {
			ev = Frame::Exit(Frame::Exit::ERROR, "SyntaxError",
							 "unknown token near `" + feed.tok + "`\n", feed.lineNumber());
			break;
		}
	} while (ev.reason == Frame::Exit::CONTINUE);


	if (ev.reason == Frame::Exit::ERROR)
		ev.genMsg(feed);

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


std::shared_ptr<Value> Frame::findVar(const std::string &name) {
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

Frame Frame::scope(const CodeFeed& feed, bool copy_stack) {
	Frame ret(feed);
	ret.prev.emplace_back(this);
	for (Frame* f : prev)
		ret.prev.emplace_back(f);

	if (copy_stack)
		ret.stack = stack; // copy stack

	return ret;
}
