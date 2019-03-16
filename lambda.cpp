
#include "frame.hpp"
#include "lambda.hpp"


Exit Lambda::call(Frame& f, std::shared_ptr<Value> slf) {

	std::vector<std::shared_ptr<Value>> args;

	// if they didn't pass any args then create an empty argument list
	if (f.stack.empty() || f.stack.back().type != Value::ARR) {
		args = std::vector<std::shared_ptr<Value>>();
	} else { // unfortunate i can't just std::move the value without fixing container:(
		args = *f.stack.back().arr;
		f.stack.pop_back();
	}

	// scope into body

	std::shared_ptr<Frame> lam = f.scope(body, false);

	// assign params
	for (size_t i = 0; i < params.size(); i++)
		lam->setVar(params[i], i + 1 > args.size() ?
					std::make_shared<Value>() : args[i]);


	// add self and arguments
	/* INSIGHT: should be a variable or definition?
	 * why definition?
		- its constant, cant change it :/
	 	-
	*/
	if (slf)
		lam->defs.emplace("self", slf);
	else if (self)
		lam->defs.emplace("self", self);
	else
		lam->defs.emplace("self", std::make_shared<Value>()); // empty

	// set args
	lam->defs.emplace("args", std::make_shared<Value>(std::move(args)));

	// capture exit value
	const Frame::Exit&& ev = lam->run(lam);

	// merge stacks
	f.stack.insert(f.stack.end(), lam->stack.begin(), lam->stack.end());

	return ev;
}

