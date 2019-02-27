#include <iostream>

#include "frame.hpp"

int main(int argc, char** argv) {


	if (argc == 1) {

		Frame frame;
		frame.feed.isStdin = true;

		while (true) {

			// reset feed so that errors don't recur
			frame.feed.reset();

			// get next line
			if (!frame.feed.getLine("> "))
				return 0;


			Frame::Exit e = frame.run();

			if (e.reason == Frame::Exit::ERROR)
				std::cout <<e.backtrace();

			else if (!frame.stack.empty())
				std::cout <<frame.stack.back().repr();

			std::cout <<std::endl;

		}
	} else if (argc == 2) {
		if (**(argv + 1) != '-') {
			Frame main;
			main.feed.loadFile(argv[1]);
			Frame::Exit e = main.run();
			if (e.reason == Frame::Exit::ERROR)
				std::cout <<e.backtrace();
		}

	}
}