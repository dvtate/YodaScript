//
// Created by tate on 20-02-19.
//

#ifndef YS2_CODE_FEED_HPP
#define YS2_CODE_FEED_HPP

#include <string>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

// basically a string wrappper
class CodeFeed {
public:


	std::string body;

	// which char are we on
	unsigned long long int offset : 63 {0};

	// if we need more lines can we read from stdin?
	bool isStdin : 1 { false };

	//
	CodeFeed(bool fromStdin=false): isStdin(fromStdin) {}

	/// add a new line to the string
	size_t getLine()
	{
		if (!isStdin)
			return false;

		std::string line;
		std::cout <<"> ";
		std::getline(std::cin, line);
		body += "\n" + line;

		return true;
	}

	void loadFile(const char* fname)
	{
		std::ifstream t(fname);
		std::stringstream buffer;
		buffer << t.rdbuf();
		body = buffer.str();
	}

	const char* cstr()
	{
		return body.c_str() + offset;
	}

	size_t lineNumber(){
		int line = 0;
		for (int i = 0; i < offset; i++)
			if (body.at(i) == '\n')
				line++;
		return line;
	}

	std::string tok;
	bool setTok() {

		// skip preceding space
		while (isspace(body.at(offset)))
			offset++;

		int i = offset;
		char c;
		std::cout <<"pulling initial token...\n";

		do {
			c = body.at(++i);
			std::cout <<i - offset << "-" << c <<"\n";
		} while (offset + i <= body.length() && !isspace(c));

		if (offset + i <= body.length())
			return false;

		this->tok = body.substr(offset, i - 1);
		std::cout <<"tok: " <<this->tok <<std::endl;

		return true;
	}
};


#endif //YS2_CODE_FEED_HPP
