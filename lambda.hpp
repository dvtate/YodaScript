//
// Created by tate on 04-03-19.
//

#ifndef YS2_LAMBDA_HPP
#define YS2_LAMBDA_HPP

#include <vector>
#include <string>
#include <memory>
#include "namespace_def.hpp"

class Frame;
class Exit;

class Lambda {
public:

	// this shold only be set for values, if self is set for a lambda within an object then
	// it will prevent the object and lambda from ever going out of scope as they will be self referencing
	std::shared_ptr<Value> self;
	std::vector<std::string> params; // args
	std::string body; // macro

	Lambda() = default;

	Exit call(Frame&, std::shared_ptr<Value> slf = nullptr);

};

#endif //YS2_LAMBDA_HPP
