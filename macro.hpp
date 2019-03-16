//
// Created by tate on 16-03-19.
//

#ifndef YS2_MACRO_HPP
#define YS2_MACRO_HPP

#include <string>

// this change is proposed if we want to have more useful error messages, but I don't think i'll implement this as it would
// degrade performance
class Macro {
public:
	// hopefully the only part that actually matters
	std::string body;

	// the file from which the macro originated
	std::string source_file;

	// from the start of the file, how many chars in is this macro?
	size_t offset;
};

#endif //YS2_MACRO_HPP
