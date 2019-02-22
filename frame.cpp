//
// Created by tate on 19-02-19.
//

#include <ctype.h>
#include "operators.hpp"
#include "frame.hpp"


Frame::Exit Frame::run()
{


	Frame::Exit ev;
	do {
		int n = 0;

		if (feed.offset + 1 >= feed.body.length())
			return Frame::Exit(Frame::Exit::FEED_END);


		// skip leading whitespace
		while (isspace(feed.body.at(feed.offset)))
			feed.offset++;

		if (feed.offset + 1 == feed.body.length()) {
			if (!feed.getLine())
				return Frame::Exit(Frame::Exit::FEED_END);
		}

		int op_ind = findOperator(*this);
		if (op_ind < 0)
			return Frame::Exit(Frame::Exit::ERROR, "SyntaxError", "unknown token on line " + feed.lineNumber());

		ev = operators[op_ind].act(*this);

	} while (ev.reason == Frame::Exit::CONTINUE);

	return ev;

}




void Frame::error(Frame::Exit e, std::string m="") {
	// go through prev to generate stacktrace
}