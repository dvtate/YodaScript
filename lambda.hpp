//
// Created by tate on 04-03-19.
//

#ifndef YS2_LAMBDA_HPP
#define YS2_LAMBDA_HPP

#include <vector>
#include <string>
#include "namespace.hpp"
class Lambda {
public:
	Namespace defs;
	std::vector<std::string> args;
	std::string body;

	Lambda(const std::vector<std::string>&& args, const std::string&& body, const Namespace&& defs):
		defs(defs), args(args), body(body) {}

};
#endif //YS2_LAMBDA_HPP
