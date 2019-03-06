//
// Created by tate on 01-03-19.
//

#include "namespace_def.hpp"
#include "value.hpp"
#include "frame.hpp"

Def::Def(const Value& value, const bool runnable):
	_val(new Value(value)), native(false), run(runnable) {}
Def::Def(Exit (*action)(Frame&)):
	native(true), act(action){}
Def::Def(const Def& def):
	native(def.native), run(def.run), act(def.act) {
	if (!native)
		_val = new Value(*def._val);
}

Def::~Def(){
	if (!native)
		delete(_val);
}