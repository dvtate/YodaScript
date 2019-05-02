//
// Created by tate on 01-03-19.
//

#include "const.hpp"

namespace op_const_empty {
	const char* name = "empty";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		// push an empty value onto the stack
		frame.stack.emplace_back(Value());
		return Frame::Exit();
	}
}
namespace op_const_null {
	const char* name = "null";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		frame.stack.emplace_back((std::shared_ptr<Value>) nullptr);
		return Frame::Exit();
	}
}
namespace op_const_true {
	const char* name = "true";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		frame.stack.emplace_back(1.0);
		return Frame::Exit();
	}
}
namespace op_const_false {
	const char* name = "false";
	bool condition(Frame& frame) {
		return frame.feed.tok == name;
	}
	Frame::Exit act(Frame& frame) {
		frame.stack.emplace_back(0.0);
		return Frame::Exit();
	}
}