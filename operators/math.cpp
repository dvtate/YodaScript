//
// Created by tate on 05-04-19.
//
#include "math.hpp"


#include <cmath>
#include <cstdio>
#include <cstdlib>


namespace math_fxns {
	Frame::Exit random(Frame &f) {
		f.stack.emplace_back((double) rand() / RAND_MAX);
		return Frame::Exit();
	}
}


const Namespace cfg_math_ns() {
	srand(time(NULL));
	return Namespace({
		{ std::string("random") , Def(math_fxns::random) },
	});
};

const Namespace math_ns = cfg_math_ns();

namespace op_math {
	const char* name = "Math";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		f.stack.emplace_back(math_ns);
		return Frame::Exit();
	}
}