//
// Created by tate on 28-03-19.
//

#include <dlfcn.h>
#include "libraries.hpp"

namespace op_load_lib {
	const char* name = "import";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", "import expected a path to an import file");

		std::string path = *f.stack.back().str;
		void* dl = dlopen(path.c_str(), RTLD_LAZY);
		if (!dl) {
			std::cout <<"err:" <<dlerror() <<std::endl;
			return Frame::Exit(Frame::Exit::ERROR, "ImportError",
							   "format or location of imported library invalid (should be .so/.dll)");
		}

		Value* ops = (Value*) dlsym(dl, "exports");
		if (ops)
			f.stack.back().set(*ops);
		else {
			std::cout <<"dlsym dlerror: " <<dlerror() <<std::endl;
		}

		/*
		auto ops = (void*(*)()) dlsym(dl, "create");

		if (ops)
			f.stack.back().set(*((Namespace*)ops()));
		else
			std::cout <<"dlerr:" <<dlerror();



		std::vector<Token>* toks = (std::vector<Token>*) dlsym(dl, "token_exports");
		operators::tokens.insert(operators::tokens.end(), toks->begin(), toks->end());

		*/
		return Frame::Exit();
	}
}


namespace op_apply_namespace {
	const char* name = "apply";
	bool condition (Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		if (f.stack.empty())
			return Frame::Exit(Frame::Exit::ERROR, "ArgError", DEBUG_FLI "apply expected a namespace", f.feed.lineNumber());

		DEFER_TOP(f);

		if (f.stack.back().type != Value::NSP)
			return Frame::Exit(Frame::Exit::ERROR, "TypeError", DEBUG_FLI "apply expected a namespace, recieved" + std::string(f.stack.back().typeName()), f.feed.lineNumber());

		for (auto& p : *f.stack.back().ns)
			f.defs.emplace(p);
		f.stack.pop_back();
		return Frame::Exit();
	}
}

namespace op_defs {
	const char* name = "defs";
	bool condition(Frame& f) {
		return f.feed.tok == name;
	}

	Frame::Exit act(Frame& f) {
		std::cout <<"Scope 0 (current):\n";
		for (const auto& def : f.defs)
			std::cout <<"\t" <<def.first << " - " <<Value(def.second).toString() <<"\n";

		size_t i = 0;
		for (; i < f.prev.size(); i++) {
			std::cout <<"Scope " <<i + 1 <<":\n";
			for (const auto& def : f.prev[i]->defs)
				std::cout <<"\t" <<def.first << " - " <<Value(def.second).toString() <<"\n";

		}

		return Frame::Exit();
	}
}