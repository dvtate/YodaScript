//
// Created by tate on 04-03-19.
//

#ifndef YS2_LAMBDA_HPP
#define YS2_LAMBDA_HPP

#include <vector>
#include <string>
#include "namespace.hpp"

// should i use a struct?
typedef struct Lambda {
	Namespace defs;
	std::vector<std::string> args;
	std::string body;
} Lambda;

#endif //YS2_LAMBDA_HPP
