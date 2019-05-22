//
// Created by tate on 16-05-19.
//

#include "errors.hpp"



namespace op_throw {
	const char *name = "throw";
	bool condition(Frame &f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame &f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "Throw expected an object/string to throw", f.feed.lineNumber());

		DEFER_TOP(f);
		Value v = f.stack.back();
		f.stack.pop_back();

		if (v.type == Value::OBJ) {
			return Frame::Exit(Frame::Exit::ERROR, "NotImplementedError", DEBUG_FLI "throw is incomplete, please only throw strings for now", f.feed.lineNumber());
		} else if (f.stack.back().type == Value::STR) {
			int colon = v.str->find(' ');
			if (colon == std::string::npos) {
				return Frame::Exit(Frame::Exit::ERROR, "ThrownError", DEBUG_FLI + *v.str, f.feed.lineNumber());
			} else {
				return Frame::Exit(Frame::Exit::ERROR, v.str->substr(0, colon), DEBUG_FLI + v.str->substr(colon), f.feed.lineNumber());
			}
		} else {
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "Throw expected an object/string to throw", f.feed.lineNumber());
		}
	}
}

namespace op_catch {
	const char *name = "catch";
	bool condition(Frame &f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame &f) {
		/* // try-catch but in reverse order bc postfix
		 * {
		 * 		# catch lambda
		 * } ($error) lambda {
		 * 		# error prone code
		 * } catch
		 */
		return Frame::Exit();
	}
}
