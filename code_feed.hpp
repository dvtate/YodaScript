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
	size_t getLine(const char* prompt = "... ")
	{

		if (!isStdin)
			return false;

		std::string line;
		std::cout <<prompt;
		if (!std::getline(std::cin, line)) // ^D
			return false;

		body += "\n" + line;

		return true;
	}

	void reset() {
		body = "";
		offset = 0;
		tok = "";
	}

	void loadFile(const char* fname) {
		std::ifstream t(fname);
		std::stringstream buffer;
		buffer <<" " <<t.rdbuf() <<"\n";
		body = buffer.str();
	}

	size_t lineNumber(){
		int line = 0;
		for (int i = 0; i < offset; i++)
			if (body.at(i) == '\n')
				line++;
		return line;
	}

	std::string tok;

	std::string fromOffset() {
		return body.substr(offset, body.length());
	}

	bool setTok() {


		//std::cout <<"stok::body[offset]: \'" <<fromOffset() <<"\'\n";
		// skip preceding space
		while (offset + 1 <= body.length() && isspace(body.at(offset)))
			offset++;
		//std::cout <<"stok::body[offset]: \'" <<fromOffset() <<"\'\n";

		if (offset >= body.length())
			return false;

		size_t i = offset;
		char c = body.at(offset);
		//std::cout <<"pulling initial token...\n";

		while (i < body.length() && !isspace(c)) {
			c = body.at(i);
			//std::cout <<i - offset << "-" << c <<"\n";
			i++;
		}

		if (i > body.length())
			return false;

		if (isspace(body[i-1]))
			i--;
		tok = body.substr(offset, i - offset);

		return true;

	}

};


#endif //YS2_CODE_FEED_HPP
