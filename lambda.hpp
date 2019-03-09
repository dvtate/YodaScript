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

// should i use a struct?
typedef struct Lambda {
	// if it's a member of something
	std::shared_ptr<Value> self;
	std::vector<std::string> params;
	std::string body;


	Exit call(Frame&);

} Lambda;

#endif //YS2_LAMBDA_HPP
