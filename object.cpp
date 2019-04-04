//
// Created by tate on 04-03-19.
//

#include "value.hpp"
#include "object.hpp"
#include "frame.hpp"


std::shared_ptr<Value>& Object::getMember(const std::string& name) {
	auto m = members.find(name);
	if (m == members.end()) {
		members.emplace(name, std::make_shared<Value>());
		m = members.find(name);
	}
	return m->second;
}

bool Object::callMember(Frame& f, const std::string& name, Exit& ev) {
	auto m = members.find(name);
	if (m == members.end())
		return false;

	if (m->second->type != Value::LAM)
		return false;

	Value lam = m->second;

	ev = m->second->lam->call(f);

	return true;
}

bool Object::callMember(Frame& f, const std::string& name, Exit& ev, const std::shared_ptr<Value>& obj, const size_t args) {
	auto m = members.find(name);
	if (m == members.end())
		return false;

	const Value* v = m->second->defer();
	if (v->type == Value::DEF)
		f.runDef(*v->def);

	if (v->type != Value::LAM)
		return false;

	Value lam_args{std::vector<std::shared_ptr<Value>>()};
	lam_args.arr->reserve(args);

	// is there enough args?
	if (args > f.stack.size()) {
		ev = Frame::Exit(Frame::Exit::ERROR, "OverloadedOperatorStackError?!", "requested more elements from stack than it contains, probably isnt ur fault", f.feed.lineNumber());
		return true;
	}

	// put args into an args list
	for (int i = 0; i < args; i++) {
		lam_args.arr->emplace_back(std::make_shared<Value>(f.stack.back()));
		f.stack.pop_back();
	}

	// pop back what should be a reference to self
	f.stack.pop_back();

	// push args list onto stack
	f.stack.emplace_back(std::move(lam_args));

	Value lam = m->second;
	ev = v->lam->call(f, obj);

	return true;
}