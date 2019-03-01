
#include <iostream>
#include <deque>



#include "namespace.hpp"

#include "operators.hpp"
#include "operators/literals.hpp"
#include "operators/ctl_flow.hpp"
#include "operators/stack_ctl.hpp"
#include "operators/types.hpp"
#include "operators/ref_ops.hpp"
#include "operators/io.hpp"
#include "operators/boolean_ops.hpp"
#include "operators/data_types.hpp"
#include "operators/const.hpp"


namespace operators {
	int findToken(Frame &f) {

		// check condition for each operator
		for (size_t i = 0; i < tokens.size(); i++) // O(N) oof
			if (tokens[i].condition(f))
				return i;

		return -1;
	}

	bool callToken(Frame& f, Frame::Exit& exit) {

		//std::cout <<"toklookup(" <<tokens.size() <<") ";
		for (Token t : tokens)
			if (t.condition(f)) {
				exit = t.act(f);
				//std::cout <<"= true\n";
				return true;
			}
		//std::cout <<"= false\n";
		return false;
	}
	Def findOperator(Frame &f) {


		auto d = operators.find(f.feed.tok);
		if (d != operators.end())
			return d->second;

		Def ret(nullptr);
		ret.run = true;
		return ret;

	}

	bool callOperator(Frame& f, Frame::Exit& exit) {

		//std::cout <<"oplookup(" <<operators.size() <<")";

		auto d = operators.find(f.feed.tok);
		if (d != operators.end()) {
			//std::cout <<" = true\n";
			if (d->second.native) {
				//std::cout <<"found: " <<d->first <<'\n';
				exit = d->second.act(f);
				return true;
			} else if (d->second.run) {

				f.stack.emplace_back(*d->second._val);
				f.feed.offset--;
				return callByName(f, "@", exit);

			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}
		//std::cout <<" = false\n";
		return false;
	}
	bool callOperator(Frame& f, Frame::Exit& exit, const Namespace& ns) {

		auto d = ns.find(f.feed.tok);
		if (d != ns.end()) {
			if (d->second.native) {
				exit = d->second.act(f);
				return true;
			} else if (d->second.run) {

				f.stack.emplace_back(*d->second._val);
				f.feed.offset--;
				return callByName(f, "@", exit);

			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}
		return false;
	}
	inline static std::deque<struct Token> genTokens() {
		// loads all operators from operators/
		std::deque<struct Token> ret = {
				OP_NS_TO_TOK(op_var_literal),

				// literals
				OP_NS_TO_TOK(op_const_macro),
				OP_NS_TO_TOK(op_const_list),
				OP_NS_TO_TOK(op_const_string),
				// comments
				OP_NS_TO_TOK(op_line_comment),
				OP_NS_TO_TOK(op_multiline_comment),

				// more literals (slower execution time
				OP_NS_TO_TOK(op_const_int),
				OP_NS_TO_TOK(op_const_number),


		};

		return ret;
	}

	inline static Namespace genOperators() {
		return Namespace({
			OP_NS_TO_PAIR(op_const_true),
			OP_NS_TO_PAIR(op_equals_to),
			OP_NS_TO_PAIR(op_exec),
			OP_NS_TO_PAIR(op_var_op),
			OP_NS_TO_PAIR(op_equals), // change the value of a variable
			OP_NS_TO_PAIR(op_set),     // give a variable a new value
			OP_NS_TO_PAIR(op_copy_value),
			OP_NS_TO_PAIR(op_vars),
			OP_NS_TO_PAIR(op_const),
			OP_NS_TO_PAIR(op_stk_clear),
			OP_NS_TO_PAIR(op_stk_dup),
			OP_NS_TO_PAIR(op_stk_swap),
			OP_NS_TO_PAIR(op_stk_pop),
			OP_NS_TO_PAIR(op_stk_size),
			OP_NS_TO_PAIR(op_repeat_loop),
			// const values
			OP_NS_TO_PAIR(op_const_empty),
			OP_NS_TO_PAIR(op_const_null),
			OP_NS_TO_PAIR(op_const_true),
			OP_NS_TO_PAIR(op_const_false),
			// cio
			OP_NS_TO_PAIR(op_println),
			OP_NS_TO_PAIR(op_print),
			OP_NS_TO_PAIR(op_input),

			// type conversion
			OP_NS_TO_PAIR(op_str),
			OP_NS_TO_PAIR(op_typeof),
			OP_NS_TO_PAIR(op_namespace),

		});
	}

	Namespace operators = genOperators();
	std::deque<struct Token> tokens = genTokens();


	bool callByName(Frame &f, const std::string &name, Frame::Exit &exit) {


		auto d = operators.find(name);
		if (d != operators.end()) {
			if (d->second.native) {
				exit = d->second.act(f);
				return true;
			} else if (d->second.run) {

				f.stack.emplace_back(*d->second._val);
				f.feed.offset--;
				return callByName(f, "@", exit);

			} else {
				f.stack.emplace_back(*d->second._val);
				return true;
			}
		}

		for (Token t : tokens)
			if (t.name == name) {
				exit = t.act(f);
				return true;
			}

		return false;

	}

}