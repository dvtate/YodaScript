//
// Created by tate on 19-02-19.
//

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
	std::stack<Value> stack;

	// defined variables
	std::map<std::string, Value*> vars;

	// free()'d at end of scope
	// ! destructor not called, don't point to complex objects
	std::vector<void*> _local_ptrs;

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

	Frame(){};
	Frame(const CodeFeed cf): feed(cf) {}


	// type returned upon evaluation of a Frame
	class Exit {
	public:

		// what is type
		enum Reason {
			ERROR = 0,	// ended in error
			CONTINUE,	// finished running successfully
			FEED_END,		// end of feed
			RETURN,		//
			BREAK,
			ESCAPE,
		} reason;

		size_t c_num;
		std::string title;
		std::string desc;

		Exit(): reason(CONTINUE) {};
		Exit(const Exit::Reason r, const std::string r_title = "", const std::string r_desc = "", const size_t char_num = 0):
				reason(r), title(r_title), desc(r_desc), c_num(char_num)
		{}
	};

	// evaluate code
	Frame::Exit run();

	// run through stacktrace adding things to msg until last elem in prev
	// for future a catch statement could be added
	void error(Exit e, std::string msg);

};

#endif //YS2_FRAME_HPP
