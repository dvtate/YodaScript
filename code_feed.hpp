#include <utility>

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
	unsigned long long int offset : 63;

	// if we need more lines can we read from stdin?
	bool isStdin : 1;

	//
	CodeFeed(bool fromStdin=false):
		offset(0), isStdin(fromStdin) {	}
	CodeFeed(std::string b):
		offset(0), isStdin(false), body(std::move(b)) { }

	/// add a new line to the string
	size_t getLine(const char* prompt = ". ")
	{

		if (!isStdin)
			return false;

		std::string line;
		std::cout <<prompt;
		if (!std::getline(std::cin, line)) // ^D
			return false;

		//std::cout <<"ADDED: \'" <<line <<"'\n";
		body += '\n';
		body += line;
		//std::cout <<"BODY: \'" <<body <<"'\n";
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

	size_t lineNumber(size_t c){
		size_t line = 0;
		for (size_t i = 0; i < c; i++)
			if (body.at(i) == '\n')
				line++;
		return line;
	}

	size_t lineNumber(){
		return lineNumber(offset);
	}

	std::string findLine(const size_t lineNum) {
		int line = 0;
		size_t start = 0;
		for (; start + 1 < body.length() && line < lineNum; start++)
			if (body.at(start) == '\n')
				line++;

		// invalid line number
		if (line != lineNum)
			return "";

		size_t end = start;
		while (end + 1< body.length()) {
			end++;
			if (body.at(end) == '\n')
				break;

		}
		//std::cout <<"start: " <<start <<"   end: " <<end <<std::endl;

		return body.substr(start, 1 + end - start);

	}

	std::string tok;

	std::string fromOffset() {
		return body.substr(offset, body.length());
	}

	// TODO: make this tokenize out member accessors
	// ie - `stack:size` --> "stack" ":size"
	bool setTok() {


		//std::cout <<"stok::body[offset]: \'" <<fromOffset() <<"\'\n";
		// skip preceding space
		while (offset + 1 <= body.length() && isspace(body.at(offset)))
			offset++;
		//std::cout <<"stok::body[offset]: \'" <<fromOffset() <<"\'\n";

		if (offset >= body.length())
			return false;

		size_t i = offset;
		char c; = body.at(offset);

		do {
			c = body.at(i);
			//std::cout <<i - offset << "-" << c <<"\n";
			i++;
		} while (i < body.length() && !isspace(c));

		if (i > body.length())
			return false;

		if (isspace(body[i - 1]))
			i--;
		tok = body.substr(offset, i - offset);

		return true;

	}

};


#endif //YS2_CODE_FEED_HPP
