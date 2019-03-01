//
// Created by tate on 01-03-19.
//

#include "namespace.hpp"
#include "value.hpp"
#include "frame.hpp"

Def::Def(const Value& value, const bool runnable):
	_val(new Value(value)), native(false), run(runnable) {}
Def::Def(Exit (*action)(Frame&)):
	native(true), act(act){}
Def::Def(const Def& def):
	native(def.native), run(def.run) {
	if (def.native)
		act = def.act;
	else
		_val = new Value(*def._val);
}

Def::~Def(){
	if (!native)
		delete(_val);
}