#include <iostream>

#include "code_feed.hpp"
#include "frame.hpp"

/*
I'm rewriting my old scripting language (YodaScript). Looking back because i made it without a plan,
 i had to add features in weird ways to make them compatible with the interpreter (ie - being forced to
 make random intermediate datatypes which had to be lazy evaluated because references were only based on
 variable names). Due to how complex and interconnected different parts of the interpreter were I was
 pretty much forced into a complete rewrite. Either way, should be fun. :)


Important internal implementation changes:
- no more 2.5k line if statement :)
- no more linked lists
- everything associated with a given scope will be contained in and handled by a Frame object
- no more running out of tempfiles
- removal of 2-3k lines of code for lazy evaluation

Important langauge changes:
- references are now technically pointers
- all variables are references to data (remember `~` == valueof)
- objects will prolly get syntax changes and improvements
  + also proper sense of self
- operator overloading (based on python)
- lambda argument handling will get changed
  + missing args handler and va_args were cool, but slow
  + will just set a var/operator to get the args the user passed

New features I'd like to explore:
- threads, new interpreter makes this more reasonable. Balancing performance might be difficult
- namespaces, for now u can just use objects
- exceptions, as the error propigates back up the scope trace,
 	+ one could have a handler defined and stop the error
- operators associated w/ primitive types
  	+ rn it doesnt make sense for them to have member fxns as they aren't objects
  	+ maybe some global namespace containing relevant lambdas/macros ?
  	+ or just including type in op name

*/

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


			if (frame.stack.size())
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