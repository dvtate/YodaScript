#include <iostream>
#include <csignal>

#include "frame.hpp"

Frame main_entry_frame;


void sigintHandle(int sig_num) {
	Frame::Exit e(Frame::Exit::ERROR, "Interrupt", "the program has been killed.", main_entry_frame.feed.lineNumber());
	e.genMsg(main_entry_frame.feed);
	std::cout <<e.backtrace();
#ifdef _WIN32
	std::cin.ignore();
#endif
	exit(sig_num);
}

int main(int argc, char** argv) {

	signal(SIGINT, sigintHandle);

	if (argc == 1) {

		main_entry_frame.feed.isStdin = true;

		while (true) {

			// reset feed so that errors don't recur
			main_entry_frame.feed.reset();

			// get next line
			if (!main_entry_frame.feed.getLine("> "))
				return 0;


			Frame::Exit e = main_entry_frame.run();

			if (e.reason == Frame::Exit::ERROR)
				std::cout <<e.backtrace();

			else if (!main_entry_frame.stack.empty())
				std::cout << main_entry_frame.stack.back().depict();

			std::cout <<std::endl;
		}
	} else if (argc == 2) {
		if (**(argv + 1) != '-') {
			main_entry_frame.feed.loadFile(argv[1]);
			Frame::Exit e = main_entry_frame.run();
			if (e.reason == Frame::Exit::ERROR) {
				std::cout <<"Entry Point: Global Line " <<e.line + 1 <<std::endl;
				std::cout <<e.backtrace();
#ifdef _WIN32
				std::cin.ignore();
#endif
			}
		}
	}
}