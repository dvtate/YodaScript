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

// type returned upon evaluation of a Frame
	// should be nested within frame but C++ is retarded and won't let you forward declare nested classes
class Exit {
public:
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

	Exit(const Exit& e):
			reason(e.reason), title(e.title), desc(e.desc), number(e.number), trace(e.trace), msg(e.msg) {}


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

class Frame {
public:
	typedef class Exit Exit;

	// functioning stack
	std::vector<Value> stack; // should never need to be resized

	// defined variables
	std::unordered_map<std::string, Value> vars;

	std::vector<Value> ref_vals; // deleted as they go out of scope

	// where is the code coming from
	CodeFeed feed;

	// previous frames (for getting vars and defs from previous scopes
	std::vector<Frame*> prev;

	// values defined by and for specific operators
	std::unordered_map<std::string, Value> rt_vals;

	// locally defined operators
	Namespace defs;
	Frame();
	Frame(const CodeFeed&);

	~Frame() = default;

	// evaluate code
	Frame::Exit run();
	Frame::Exit runDef(const Def& def);
	Frame scope(const CodeFeed& feed, bool copy_stack = true);

	// if var found return it's data reference
	std::shared_ptr<Value> getVar(const std::string&); // if var is in previous scope, set it to default ref previous scoped variable
	std::shared_ptr<Value> findVar(const std::string& name); // find var from prev scopes

};


class Frame_Exit;

#endif //YS2_FRAME_HPP
