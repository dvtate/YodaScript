#ifndef YS2_FRAME_HPP
#define YS2_FRAME_HPP

#include <cstdlib>
#include <stack>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include "value.hpp"
#include "code_feed.hpp"


class Frame {
public:
	// functioning stack
	std::vector<Value> stack; // should never need to be resized

	// defined variables
	std::map<std::string, Value*> vars;

	// free()'d at end of scope
	// ! destructor not called, don't point to complex objects
	std::vector<void*> _local_ptrs;

	// values referenced in current scope
	std::vector<Value*> ref_vals;

	// where is the code coming from
	CodeFeed feed;

	// previous frames (for getting vars and defs from previous scopes
	std::vector<Frame*> prev;

	// values defined by and for specific operators
	std::unordered_map<std::string, Value*> rt_vals;

	~Frame()
	{
		// free locals
		for (void* p : _local_ptrs)
			free(p);

	}

	Frame(){
		stack.reserve(30);
	}
	Frame(const CodeFeed cf): feed(cf) {
		stack.reserve(30);
	}


	// type returned upon evaluation of a Frame
	class Exit {
	public:

		// what is type
		enum Reason {
			ERROR = 0,	// ended in error
			CONTINUE,	// finished running successfully
			FEED_END,	// end of feed
			RETURN,		//
			BREAK,		//
			ESCAPE,
		} reason;

		/*
		 * if error: keep going up frames until one has a handler
		 * if escape(n): subtract one from line and go up, if
		 */

		std::string title;
		std::string desc;

		// same variable
		union { size_t line; size_t number; };

		std::vector<Exit> trace{};

		Exit(): reason(CONTINUE) {};
		Exit(const Exit::Reason r, const std::string r_title = "", const std::string r_desc = "", const size_t line_num = 0):
				reason(r), title(r_title), desc(r_desc), line(line_num)
		{}
		Exit(const Exit::Reason r, const std::string r_title, const std::string r_desc, const size_t line_num, const Exit& e):
				reason(r), title(r_title), desc(r_desc), line(line_num)
		{
			trace.push_back(e);
			for (Exit ev : e.trace)
				trace.push_back(ev);
		}

		Exit(const Exit& e) {
			reason = e.reason;
			title = e.title;
			desc = e.desc;
			number = e.number;
			trace = e.trace;
			msg = e.msg;
		}


		std::string msg{""};
		void genMsg(CodeFeed& feed) {
			msg += "\nLocal Line: " + std::to_string(line + 1);
			msg += "\n" + title + ": " + desc + "\nnear: " + feed.findLine(line) + '\n';
		}

		std::string backtrace() {
			std::string ret = msg;
			for (Exit e : trace)
				ret += "\n" + e.msg;
			return ret;
		}
	};

	// evaluate code
	Frame::Exit run();

};

#endif //YS2_FRAME_HPP
