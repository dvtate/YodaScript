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

class Exit;

// TODO: make all frames std::shared_ptrs so that it's friendly to multi-threaded design in future
//		 also adding a scope & scopes operator for accessing Frame.defs within context of a namespace

class Frame {
public:
	typedef class Exit Exit;

	// set to refrence self when being run/making scopes off of it
	std::shared_ptr<Frame> self_ref{nullptr};

	// functioning stack
	std::vector<Value> stack; // should never need to be resized

	// defined variables
	std::unordered_map<std::string, Value> vars;

	std::vector<Value> ref_vals; // deleted as they go out of scope

	// where is the code coming from
	CodeFeed feed;

	// previous frames (for getting vars and defs from previous scopes
	// TODO: convert to use std::shared_ptr!
	//std::vector<Frame*> prev;
	std::vector<std::shared_ptr<Frame>> prev;

	// values defined by and for specific operators
	std::unordered_map<std::string, Value> rt_vals;

	// TODO: make this a shared_ptr and allow operators from previous scopes to call it
	//			this would allow user to call redefined/global operators
	// locally defined operators
	Namespace defs;

	Frame();
	explicit Frame(const CodeFeed&);

	~Frame() = default;

	// evaluate code
	Frame::Exit run(std::shared_ptr<Frame>& self);
	Frame::Exit runDef(const Def& def);
	std::shared_ptr<Frame> scope(const CodeFeed&& cfeed, bool copy_stack = true);

	// if var found return it's data reference
	std::shared_ptr<Value> getVar(const std::string&); // if var is in previous scope, set it to default ref previous scoped variable
	std::shared_ptr<Value> findVar(const std::string& name); // find var from prev scopes
	std::shared_ptr<Value> setVar(const std::string& name, const std::shared_ptr<Value>& val); // set var, found or not

	// search through defined variables for the one referenced
	std::string varName(const std::shared_ptr<Value>& ref);
};


// type returned upon evaluation of a Frame
// should be nested within Frame but C++ is retarded and won't let you forward declare nested classes
class Exit {
public:
	enum Reason {
		CONTINUE = 0,	// finished running successfully
		ERROR,		// ended in error
		FEED_END,	// end of feed
		RETURN,		// escape current lambda/macro?
		ESCAPE,		// go up one frame
		UP,			// go up n frames
	} reason;

	/* TODO: catch expressions
	 * if error: keep going up frames until one has a handler
	 * if up(n): subtract one from line and go up, if
	 */

	std::string title;
	std::string desc;

	// same variable
	union { ssize_t line; ssize_t number; };

	std::vector<Exit> trace;

	Exit(): reason(CONTINUE) {};
	Exit(const Exit::Reason r, const std::string& r_title = "", const std::string& r_desc = "", const size_t line_num = 0):
			reason(r), title(r_title), desc(r_desc), line(line_num)
	{}
	Exit(const Exit::Reason r, const std::string& r_title, const std::string& r_desc, const size_t line_num, const Exit& e):
			reason(r), title(r_title), desc(r_desc), line(line_num)
	{
		trace.emplace_back(e);
		for (const Exit& ev : e.trace)
			trace.emplace_back(ev);
	}

	Exit(const Exit& e) = default;


	std::string msg{""};
	void genMsg(CodeFeed& feed, Frame* frm = nullptr) {
		feed.offset -= feed.tok.length();
		msg += "\n" + title + ": " + desc; // red
		msg += "\nnear local line "  + std::to_string(line + 1) +  " :" + feed.findLine(line); // green

		if (frm) {
			std::vector<std::shared_ptr<Value>> stklst;
			for (Value &v : frm->stack)
				stklst.emplace_back(std::make_shared<Value>(v));
			msg += "\nstack: " + Value(stklst).depict();
		}
		msg += '\n';

		feed.offset += feed.tok.length();
	}

	std::string backtrace() {
		std::string ret = msg;
		for (Exit e : trace)
			ret += e.msg;
		return ret;
	}
};


extern std::shared_ptr<Frame> main_entry_frame;

#endif //YS2_FRAME_HPP
