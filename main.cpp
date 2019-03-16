#include <iostream>
#include <csignal>
#include <cstring>

#include "frame.hpp"

std::shared_ptr<Frame> shared_main_entry_frame = std::make_shared<Frame>();
Frame& main_entry_frame = *shared_main_entry_frame;


void sigintHandle(int sig_num) {
	Frame::Exit e(Frame::Exit::ERROR, "Interrupt", "the program has been killed.", main_entry_frame.feed.lineNumber());
	e.genMsg(main_entry_frame.feed);
	std::cout <<e.backtrace();
#ifdef _WIN32
	std::cin.ignore();
#endif
	exit(sig_num);
}

// trace on
// print out every single token
bool enable_token_trace = false;

extern const char* ys_help_info;
extern const char* ys_version_info;


int main(int argc, char** argv) {

	signal(SIGINT, sigintHandle);

	// this entire file should prolly be reworked, works for now tho so idc

	bool from_file = false;
	for (int i = 1; i < argc; i++)
		if (argv[i][0] != '-') {
			from_file = true;
		} else if (!strcmp(argv[i], "--tron") || !strcmp(argv[i], "-t")) {
			enable_token_trace = true;
		} else if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-V")) {
			std::cout <<ys_version_info <<std::endl;
			return 0;
		} else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			std::cout <<ys_help_info <<std::endl;
			return 0;
		}


	if (!from_file) {

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
	}

	// else -> run file

	// run program
	for (int i = 1; i < argc; i++)
		if (**(argv + i) != '-') {
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

const char* ys_version_info = "2.0.1 - compiled: " __DATE__ " " __TIME__;

const char* ys_help_info =
"usage: Usage: yoda [ option | file ] ...\n"
"Options:\n"
"   -h,\t--help\t: display's this help message\n"
"   -V,\t--version\t: display's version information \n"
"   -t,\t--tron\t: prints every token as it's interpreted\n"
"\n"
"If this is your first time using this language, you should check the README on this project's github page.\n"
"<https://github.com/dvtate/ys-alpha>\n";