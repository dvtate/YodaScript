#include "../extend.hpp"
#include "../frame.hpp"

// $ g++ lib.cpp ../namespace_def.cpp ../value.cpp -lgmp -lgmpxx -fPIC -shared -o lib.so

Exit greet(Frame& f) {
	std::cout <<"ff\n";
	return Exit();
}

Value exports = Namespace({
	{ "hi", Def(greet) }
});
