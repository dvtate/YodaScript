
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

		//std::cout <<"framerun::body[offset]: \'" <<feed.fromOffset() <<"\'\n";
		ev = operators[op_ind].act(*this);

	} while (ev.reason == Frame::Exit::CONTINUE);


	if (ev.reason == Frame::Exit::ERROR)
		ev.genMsg(feed);

	//std::cout <<ev.msg;
	return ev;

}
