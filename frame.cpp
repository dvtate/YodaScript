
#include <ctype.h>
#include "operators.hpp"
#include "frame.hpp"


Frame::Exit Frame::run()
{
	//std::cout <<"running line: " <<feed.body <<std::endl;

	Frame::Exit ev;
	do {

		//std::cout <<"framerun::body[offset]: \'" <<feed.fromOffset() <<"\'\n";
		int op_ind = findOperator(*this);
		if (op_ind == -1) {
			ev = Frame::Exit(Frame::Exit::ERROR, "SyntaxError",
							 "unknown token on line " + std::to_string(feed.lineNumber()) + " near `" + feed.tok +
							 "`\n", feed.lineNumber());
			break;
		}
		if (op_ind == -2 || feed.offset >= feed.body.length())
			return Frame::Exit(Frame::Exit::FEED_END);

		ev = operators[op_ind].act(*this);

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